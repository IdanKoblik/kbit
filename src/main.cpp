#include "cmd/command_factory.h"
#include "file_handler.h"
#include <memory>
#include <iostream>
#include <vector>

static const std::vector<std::string> art = {
    "----    ---- -----------  --------  ------------",
    "****   ****  ***********  ********  ************",
    "----  ----   ----       -   ----    ------------",
    "*********    ***********    ****        ****    ",
    "---------    -----------    ----        ----    ",
    "****  ****   ****       *   ****        ****    ",
    "----   ----  -----------  --------      ----    ",
    "****    **** ***********  ********      ****    "
};

int main(int argc, char** argv) {
    for (const std::string& line : art)
        std::cout << line << std::endl;

    std::cout << "\n";
    if (argc == 3) {
        std::unique_ptr<Command> cmd = get_command(argv[1]);
        if (cmd) {
            cmd->handle(argv);
            return 0;
        }
    }

    return 0;
}
