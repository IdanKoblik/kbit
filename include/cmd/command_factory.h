#ifndef COMMAND_FACTORY
#define COMMAND_FACTORY

#include <memory>
#include "command.h"

std::unique_ptr<Command> get_command(const std::string& cmd);

#endif // COMMAND_FACTORY