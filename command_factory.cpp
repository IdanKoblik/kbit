#include "command_factory.h"

#include "decode_command.h"

std::unique_ptr<Command> get_command(const std::string& cmd) {
    if (cmd == "decode")
        return std::make_unique<DecodeCommand>();

    std::cerr << "Unknown command: " << cmd << std::endl;
    return nullptr;
}