#ifndef RESOLVE_H
#define RESOLVE_H

#include <string>
#include <stdint.h>

namespace net {

    struct ConnectionInfo {
        std::string host;
        uint16_t port;
    };

    ConnectionInfo resolve(const std::string& host, uint16_t port = 80);
}

#endif // RESOLVE_H
