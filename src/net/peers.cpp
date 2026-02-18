#include "net/peers.h"

#include <klogger/logger.h>
#include "net/resolve.h"
#include "net/tcp_wrapper.h"
#include "parser/decoder.h"
#include "parser/bencode.h"
#include "file_handler.h"

static std::string urlEncode(const std::string& data) {
    std::ostringstream encoded;

    for (unsigned char c : data) {
        encoded << "%"
                << std::uppercase
                << std::hex
                << std::setw(2)
                << std::setfill('0')
                << static_cast<int>(c);
    }

    return encoded.str();
}

namespace net {

void discover(const std::string& trackerURL, TorrentFile& torrent) {
    if (trackerURL.rfind("http://", 0) != 0)
        return;

    std::string url = trackerURL.substr(7);
    size_t slashPos = url.find('/');
    std::string hostPort = url.substr(0, slashPos);
    std::string path = (slashPos == std::string::npos) ? "/" : url.substr(slashPos);

    std::string host;
    uint16_t port = 80;

    size_t colonPos = hostPort.find(':');
    if (colonPos != std::string::npos) {
        host = hostPort.substr(0, colonPos);
        port = static_cast<uint16_t>(
            std::stoi(hostPort.substr(colonPos + 1))
        );
    } else {
        host = hostPort;
    }

    net::ConnectionInfo info = net::resolve(host, port);

    net::TcpWrapper tcp;
    if (!tcp.connect(info)) {
        LOG_ERROR("Failed to connect to tracker " + trackerURL);
        return;
    }

    std::string peerId = "-KB0001-123456789012"; // must be 20 bytes

    std::string request =
        "/announce?info_hash=" + urlEncode(torrent.infoHash) +
        "&peer_id=" + urlEncode(peerId) +
        "&port=6881"
        "&uploaded=0"
        "&downloaded=0"
        "&left=" + std::to_string(torrent.length) +
        "&compact=1";

    std::string response  = tcp.getRequest(request);
    size_t bodyPos = response.find("\r\n\r\n");
    if (bodyPos == std::string::npos)
        return;
    std::string body = response.substr(bodyPos + 4);

    size_t pos = 0;
    BencodeValue decoded = decode(body, pos);
    BencodeDict dict = asDict(decoded);

    const std::string& peersStr = asString(dict.at("peers"));

    for (size_t i = 0; i + 6 <= peersStr.size(); i += 6) {
        std::string ip = std::to_string(static_cast<uint8_t>(peersStr[i])) + "." +
            std::to_string(static_cast<uint8_t>(peersStr[i+1])) + "." +
            std::to_string(static_cast<uint8_t>(peersStr[i+2])) + "." +
            std::to_string(static_cast<uint8_t>(peersStr[i+3]));

        uint16_t peerPort = (static_cast<uint8_t>(peersStr[i+4]) << 8) |
            static_cast<uint8_t>(peersStr[i+5]);

        LOG_INFO("Found Peer: " + ip + ":" + std::to_string(peerPort));
        net::ConnectionInfo info = {
            .host = ip,
            .port = peerPort
        };

        torrent.peers.insert(info);
    }
}
};
