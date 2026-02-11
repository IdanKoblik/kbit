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
          std::cout << torrent->infoHash << std::endl;

          return true;
      } catch (const std::exception& e) {
          LOG_EXCEPTION(e);
          LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
      }

      return false;
  }
};

#endif // PARSE_COMMAND
