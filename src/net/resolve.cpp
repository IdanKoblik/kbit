#include "net/resolve.h"

#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <string>
#include <klogger/logger.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/x509v3.h>

namespace net {
    ConnectionInfo resolve(const std::string& host, uint16_t port) {
        struct addrinfo hints{}, *res;
        std::memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        ConnectionInfo info{};
        std::string port_str = std::to_string(port);
        int status = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
        if (status != 0) {
            std::string str(gai_strerror(status));
            LOG_ERROR("getaddrinfo error: " + str);
            return ConnectionInfo{};
        }

        for (struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
            char host_buf[NI_MAXHOST];
            char port_buf[NI_MAXSERV];

            int result = getnameinfo(
                p->ai_addr,
                p->ai_addrlen,
                host_buf, sizeof(host_buf),
                port_buf, sizeof(port_buf),
                NI_NUMERICHOST | NI_NUMERICSERV
            );

            if (result != 0)
                continue;

            info.host = host_buf;
            info.port = static_cast<uint16_t>(std::stoi(port_buf));
        }

        freeaddrinfo(res);
        return info;
    };
}
