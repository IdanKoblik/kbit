#ifndef RESOLVE_H
#define RESOLVE_H

#include <string>
#include <stdint.h>

namespace net {

    struct ConnectionInfo {
        std::string host;
        uint16_t port;

        bool operator==(const ConnectionInfo& other) const {
            return host == other.host && port == other.port;
        }
    };

    ConnectionInfo parseAddress(const std::string& address);
    ConnectionInfo resolve(const std::string& host, uint16_t port = 80);
};

#endif // RESOLVE_H
