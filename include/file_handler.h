#ifndef FILE_HANDLER
#define FILE_HANDLER

#include <memory>
#include <string>
#include <stdint.h>
#include <vector>

struct TorrentFile {
    std::string trackerURL{};
    std::string infoHash{};
    bool isPrivate{false};

    std::vector<std::string> hashes;
};

std::unique_ptr<TorrentFile> parseTorrent(const std::string& path);

#endif // FILE_HANDLER
