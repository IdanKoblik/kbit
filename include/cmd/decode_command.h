#ifndef DECODE_COMMAND
#define DECODE_COMMAND

#include "command.h"
#include "parser/decoder.h"

class DecodeCommand : public Command {
public:
    DecodeCommand() {
        this->name = "decode";
    }

    bool handle(char** argv) override {
        if (!argv[2]) {
            std::cerr << "Error: Missing input for decode command.\n";
            return false;
        }

        size_t i = 0;
        BencodeValue decoded = decode(argv[2], i);
        print_bencode(decoded, 0);
        return true;
    }
};

#endif // DECODE_COMMAND