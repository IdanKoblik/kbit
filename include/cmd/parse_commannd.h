#ifndef PARSE_COMMAND
#define PARSE_COMMAND

#include <klogger/logger.h>
#include "command.h"
#include "file_handler.h"

class ParseCommand : public Command {
public:
  ParseCommand() {
      this->name = "parse";
  }

  bool handle(char **argv) override {
      if (!argv[2]) {
          LOG_ERROR("Missing input for parse command.");
          return false;
      }

      try {
          std::unique_ptr<TorrentFile> torrent = parseTorrent(argv[2]);
          std::cout << torrent->trackerURL << std::endl;
          std::cout << (torrent->isPrivate ? "Private" : "Public") << std::endl;
          std::cout << "Len: " + std::to_string(torrent->length) << std::endl;

          std::ostringstream hexOut;
          hexOut << std::hex << std::setfill('0');
          for (unsigned char c : torrent->infoHash)
              hexOut << std::setw(2) << static_cast<int>(c);

          std::cout << "Info Hash: " << hexOut.str() << std::endl;
          for (const auto& peer : torrent->peers)
              std::cout << "Peer: " << peer.host << ":" << peer.port << std::endl;

          return true;
      } catch (const std::exception& e) {
          LOG_EXCEPTION(e);
          LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
      }

      return false;
  }
};

#endif // PARSE_COMMAND
