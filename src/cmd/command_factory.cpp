#include "cmd/command_factory.h"

#include "cmd/decode_command.h"
#include "cmd/parse_commannd.h"

std::unique_ptr<Command> get_command(const std::string& cmd) {
    if (cmd == "decode")
        return std::make_unique<DecodeCommand>();

    if (cmd == "parse")
        return std::make_unique<ParseCommand>();

    std::cerr << "Unknown command: " << cmd << std::endl;
    return nullptr;
}
