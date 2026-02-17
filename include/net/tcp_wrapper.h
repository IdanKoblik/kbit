#ifndef TCP_WRAPPER_H
#define TCP_WRAPPER_H

#include "resolve.h"
#include <string>
#include <stdint.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace net {

    class TcpWrapper {
        private:
            int socket = -1;
            ConnectionInfo info;
            SSL_CTX* sslCtx = nullptr;
            SSL* sslHandle = nullptr;
            bool ssl = false;

            void initSSL();

        public:
            TcpWrapper(bool ssl = false);
            ~TcpWrapper();

            bool connect(const ConnectionInfo& info, bool useSSL = false);

            bool send(const std::string& data);
            std::string receive(size_t bufferSize = 4096);

            std::string getRequest(const std::string& path);
            std::string postRequest(const std::string& path, const std::string& body, const std::string& contentType = "application/x-www-form-urlencoded");
    };

}

#endif // TCP_WRAPPER_H
