#include "file_handler.h"

#include "parser/decoder.h"
#include "parser/encoder.h"
#include <exception>
#include <fstream>
#include <iterator>
#include <klogger/logger.h>
#include <openssl/sha.h>
#include <iostream>

static BencodeDict asDict(const BencodeValue& v) {
    if (!std::holds_alternative<BencodeDict>(v.value))
        throw std::runtime_error("BencodeValue is not a dictionary!");

    return std::get<BencodeDict>(v.value);
}

static BencodeList asList(const BencodeValue& v) {
    if (!std::holds_alternative<BencodeList>(v.value))
        throw std::runtime_error("BencodeValue is not a list!");

    return std::get<BencodeList>(v.value);
}

static const std::string& asString(const BencodeValue& v) {
    return std::get<std::string>(v.value);
}

static std::string sha1Binary(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(data.data()), data.size(), hash);

    return std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
}

std::unique_ptr<TorrentFile> parseTorrent(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        LOG_ERROR("Target file was not found");
        return nullptr;
    }

    std::string data = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    size_t pos = 0;

    try {
        BencodeValue decoded = decode(data, pos);
        const BencodeDict root = asDict(decoded);
        const BencodeDict infoDict = asDict(root.at("info"));

        std::unique_ptr<TorrentFile> torrent = std::make_unique<TorrentFile>();
        torrent->trackerURL = asString(root.at("announce"));

        const std::string infoEncoded = encode(infoDict);
        const std::string infoHash = sha1Binary(infoEncoded);
        std::ostringstream hexOut;
        hexOut << std::hex << std::setfill('0');
        for (unsigned char c : infoHash)
            hexOut << std::setw(2) << static_cast<int>(c);

        torrent->infoHash = hexOut.str();

        const std::string& piecesStr = asString(infoDict.at("pieces"));
        if (piecesStr.size() % SHA_DIGEST_LENGTH != 0)
            throw std::runtime_error("Invalid pieces length in torrent file");

        torrent->hashes.reserve(piecesStr.size() / SHA_DIGEST_LENGTH);
        for (size_t i = 0; i < piecesStr.size(); i += SHA_DIGEST_LENGTH) {
            const std::string piece = piecesStr.substr(i, SHA_DIGEST_LENGTH);
            std::ostringstream pieceHex;
            pieceHex << std::hex << std::setfill('0');
            for (unsigned char c : piece)
                pieceHex << std::setw(2) << static_cast<int>(c);
            torrent->hashes.push_back(pieceHex.str());
        }

        return torrent;
    } catch (const std::exception& e) {
        LOG_EXCEPTION(e);
        LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
    }

    return nullptr;
}
