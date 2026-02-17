#include "net/tcp_wrapper.h"

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
#include <vector>

namespace net {

    TcpWrapper::TcpWrapper(bool ssl) {
        if (ssl) {
            initSSL();
            this->ssl = true;
        }
    }

    void TcpWrapper::initSSL() {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        this->sslCtx = SSL_CTX_new(TLS_client_method());
    }

    TcpWrapper::~TcpWrapper() {
        if (this->sslHandle && this->ssl) {
            SSL_shutdown(this->sslHandle);
            SSL_free(this->sslHandle);
        }

        if (this->socket > 0)
            ::close(this->socket);

        this->socket = -1;
        if (this->sslCtx && this->ssl)
            SSL_CTX_free(this->sslCtx);
    }


    bool TcpWrapper::connect(const ConnectionInfo& info, bool useSSL) {
        struct sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(info.port);
        inet_pton(AF_INET, info.host.c_str(), &server_addr.sin_addr);

        this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket < 0) {
            LOG_ERROR("Failed to create socket");
            return false;
        }

        if (::connect(socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            LOG_ERROR("Failed to connect to " + info.host + ":" + std::to_string(info.port));
            return false;
        }

        this->info = info;
        if (!useSSL) {
            LOG_INFO("Successfully connected to " + info.host + ":" + std::to_string(info.port));
            LOG_DEBUG("TEST: " + std::to_string(this->socket));
            return true;
        }

        if (this->ssl && useSSL) {
            this->sslHandle = SSL_new(this->sslCtx);
            SSL_set_tlsext_host_name(this->sslHandle, info.host.c_str());
            SSL_set_fd(this->sslHandle, this->socket);

            if (SSL_connect(this->sslHandle) <= 0) {
                LOG_ERROR("TLS Handshake failed");
                return false;
            }

            LOG_INFO("Successfully connected to " + info.host + ":" + std::to_string(info.port) + ", with SSL");
            return true;
        }

        LOG_ERROR("SSL support disabled");
        return false;
    };

    bool TcpWrapper::send(const std::string& data) {
        if (this->socket < 0) {
            LOG_ERROR("Connection was not established");
            return false;
        }

        if (this->ssl) {
            if (!this->sslHandle) {
                LOG_ERROR("You tried to send https request without starting ssl connection");
                return false;
            }

            return SSL_write(this->sslHandle, data.c_str(), data.length());
        }

        return ::send(this->socket, data.c_str(), data.length(), 0);
    }

    std::string TcpWrapper::receive(size_t bufferSize) {
        std::string result;
        std::vector<char> buffer(bufferSize);

        int bytes = 0;

        if (this->ssl && this->sslHandle) {
            while (true) {
                bytes = SSL_read(this->sslHandle, buffer.data(), bufferSize);

                if (bytes > 0) {
                    result.append(buffer.data(), bytes);
                } else {
                    break;
                }
            }
        } else {
            while (true) {
                bytes = ::recv(this->socket, buffer.data(), bufferSize, 0);
                if (bytes > 0) {
                    result.append(buffer.data(), bytes);
                } else if (bytes == 0) {
                    // connection closed normally
                    break;
                } else {
                    perror("recv failed");
                    break;
                }
            }
        }

        return result;
    }

    std::string TcpWrapper::getRequest(const std::string& path) {
        std::string request =
            "GET " + path + " HTTP/1.1\r\n"
            "Host: " + this->info.host + "\r\n"
            "Connection: close\r\n\r\n";

        if (!this->send(request)) {
            LOG_ERROR("Failed to send a GET request");
            return std::string{};
        }

        return this->receive();
    }

    std::string TcpWrapper::postRequest(const std::string& path, const std::string& body, const std::string& contentType) {
        std::string request =
            "POST " + path + " HTTP/1.1\r\n"
            "Host: " + this->info.host + "\r\n"
            "Content-Type: " + contentType + "\r\n"
            "Content-Length: " + std::to_string(body.length()) + "\r\n"
            "Connection: close\r\n\r\n"
            + body;

        if (!this->send(request)) {
            LOG_ERROR("Failed to send a POST request");
            return std::string{};
        }

        return this->receive();
    }
}
