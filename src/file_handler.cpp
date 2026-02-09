#include "file_handler.h"

#include "parser/decoder.h"
#include <exception>
#include <fstream>
#include <iterator>
#include <klogger/logger.h>

static BencodeDict asDict(const BencodeValue &v) {
    if (!std::holds_alternative<BencodeDict>(v.value)) {
        throw std::runtime_error("BencodeValue is not a dictionary!");
    }

    return std::get<BencodeDict>(v.value);
}

static const std::string &asString(const BencodeValue &v) {
    return std::get<std::string>(v.value);
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

        std::unique_ptr<TorrentFile> torrent = std::make_unique<TorrentFile>();
        torrent->trackerURL = asString(asDict(decoded).at("announce"));

        return torrent;
    } catch (const std::exception& e) {
        LOG_EXCEPTION(e);
        LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
    }

    return nullptr;
}
