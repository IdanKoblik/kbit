#ifndef PARSE_COMMAND
#define PARSE_COMMAND

#include <fstream>
#include <klogger/logger.h>
#include "command.h"
#include "parser/decoder.h"

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

      std::ifstream file(argv[2], std::ios::binary);
      if (!file) {
          LOG_ERROR("Target file was not found");
          return false;
      }

      std::string data = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
      size_t pos = 0;

      try {
          BencodeValue decoded = decode(data, pos);
          print_bencode(decoded, 0);
          return true;
      } catch (const std::exception& e) {
          LOG_EXCEPTION(e);
          LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
      }

      return false;
  }
};

#endif // PARSE_COMMAND
