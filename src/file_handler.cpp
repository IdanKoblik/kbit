#include "file_handler.h"

#include "net/peers.h"
#include "parser/bencode.h"
#include "parser/decoder.h"
#include "parser/encoder.h"
#include <exception>
#include <fstream>
#include <iterator>
#include <klogger/logger.h>
#include <memory>
#include <openssl/sha.h>
#include <iostream>
#include <thread>
#include <future>
#include <vector>

static std::string sha1Binary(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(data.data()), data.size(), hash);

    return std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
}

static std::string printSha(const std::string& hash) {
    std::ostringstream hexOut;
    hexOut << std::hex << std::setfill('0');
    for (unsigned char c : hash)
        hexOut << std::setw(2) << static_cast<int>(c);
    return hexOut.str();
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
        LOG_INFO("Tracker URL: " + torrent->trackerURL);

        auto lenIt = infoDict.find("length");
        if (lenIt != infoDict.end()) {
            // Single file mode
            torrent->length = asInt(lenIt->second);
        } else {
            // Multi-file mode
            auto filesIt = infoDict.find("files");
            if (filesIt == infoDict.end())
                throw std::runtime_error("Torrent missing length and files");

            const BencodeList& files = asList(filesIt->second);

            torrent->length = 0;

            for (const BencodeValue& file : files) {
                const BencodeDict& fileDict = asDict(file);
                torrent->length += asInt(fileDict.at("length"));
            }
        }

        LOG_INFO("Total torrent size: " + std::to_string(torrent->length) + " bytes");
        auto pit = infoDict.find("private");
        torrent->isPrivate =
            (pit != infoDict.end() && asInt(pit->second) == 1);

        LOG_INFO("Torrent is " + std::string(torrent->isPrivate ? "private" : "public"));
        const std::string infoEncoded = encode(infoDict);
        torrent->infoHash = sha1Binary(infoEncoded); // RAW 20 bytes

        LOG_INFO("Info hash: " + printSha(torrent->infoHash));
        const std::string& piecesStr = asString(infoDict.at("pieces"));
        if (piecesStr.size() % SHA_DIGEST_LENGTH != 0)
            throw std::runtime_error("Invalid pieces length");

        for (size_t i = 0; i < piecesStr.size(); i += SHA_DIGEST_LENGTH) {
            torrent->hashes.push_back(
                piecesStr.substr(i, SHA_DIGEST_LENGTH)
            );
        }

        auto it = root.find("announce-list");
        if (it != root.end()) {
            std::vector<std::future<void>> futures;

            for (const BencodeValue& tier : asList(it->second)) {
                for (const BencodeValue& tracker : asList(tier)) {
                    const std::string& url = asString(tracker);
                    if (url.rfind("http://", 0) != 0 && url.rfind("https://", 0) != 0) {
                        LOG_WARN("Skipping non-HTTP tracker: " + url);
                        continue;
                    }

                    if (torrent->isPrivate) {
                        net::discover(url, *torrent);
                        continue;
                    }

                    futures.push_back(std::async(std::launch::async, [url, &torrent]() {
                        try {
                            net::discover(url, *torrent);
                        } catch (const std::exception& e) {
                            LOG_ERROR("Discovery failed for " + url + ": " + e.what());
                        }
                    }));
                }
            }

            for (auto& f: futures)
                f.wait();
        }

        return torrent;
    } catch (const std::exception& e) {
        LOG_EXCEPTION(e);
        LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
    }

    return nullptr;
}
