#ifndef PEERS_H
#define PEERS_H

#include <string>
#include "file_handler.h"

namespace net {

    void discover(const std::string& trackerURL, TorrentFile& torrent);
};

#endif // PEERS_H
