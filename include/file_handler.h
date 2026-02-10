#ifndef FILE_HANDLER
#define FILE_HANDLER

#include <memory>
#include <string>
#include <stdint.h>

struct TorrentFile {
    std::string trackerURL{};
    std::string infoHash{};
};

std::unique_ptr<TorrentFile> parseTorrent(const std::string& path);

#endif // FILE_HANDLER
