#ifndef DECODE_COMMAND
#define DECODE_COMMAND

#include <klogger/logger.h>
#include "command.h"
#include "parser/decoder.h"

class DecodeCommand : public Command {
public:
    DecodeCommand() {
        this->name = "decode";
    }

    bool handle(char** argv) override {
        if (!argv[2]) {
            LOG_ERROR("Missing input for decode command.");
            return false;
        }


        size_t pos = 0;
        try {
            BencodeValue decoded = decode(argv[2], pos);
            print_bencode(decoded, 0);
            return true;
        } catch (std::exception& e) {
            LOG_EXCEPTION(e);
            LOG_FATAL("Failed to decode torrent file" + std::string(e.what()));
        }

        return false;
    }
};

#endif // DECODE_COMMAND
