#ifndef FILE_HANDLER
#define FILE_HANDLER

#include "net/resolve.h"
#include <memory>
#include <string>
#include <stdint.h>
#include <vector>
#include <unordered_set>

namespace std {
template <>
struct hash<net::ConnectionInfo> {
    size_t operator()(const net::ConnectionInfo& c) const noexcept {
        size_t h1 = std::hash<std::string>{}(c.host);
        size_t h2 = std::hash<uint16_t>{}(c.port);
        return h1 ^ (h2 << 1);
    }
};
};

struct TorrentFile {
    std::string trackerURL{};
    std::string infoHash{};
    bool isPrivate{false};
    long long length{0};

    std::unordered_set<net::ConnectionInfo> peers;
    std::vector<std::string> hashes;
};

std::unique_ptr<TorrentFile> parseTorrent(const std::string& path);

#endif // FILE_HANDLER
