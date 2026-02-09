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

    if (argc < 2)
        return 0;

    if (argc == 2) {
        std::unique_ptr<TorrentFile> file = parseTorrent(argv[1]);
        std::string test = file->trackerURL;
        printf("%s", test.c_str());
        return 0;
    }

    if (argc == 3) {
        std::unique_ptr<Command> cmd = get_command(argv[1]);
        if (cmd) {
            cmd->handle(argv);
            return 0;
        }
    }

    return 0;
}
