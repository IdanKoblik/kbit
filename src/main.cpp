#include "cmd/command_factory.h"
#include "net/tcp_wrapper.h"
#include "net/resolve.h"
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

    net::TcpWrapper* tcp = new net::TcpWrapper();

    // TODO remove
    net::ConnectionInfo info = {
        .host = "127.0.0.1",
        .port = 8000
    };

    std::string test = tcp->connect(info) ? "Connection successful" : "Connection failed";
    std::cout << test << std::endl;

    std::string res = tcp->getRequest("/hello?name=idan");
    std::cout << "Response:\n" << res << std::endl;

    std::cout << "\n";
    if (argc == 3) {
        std::unique_ptr<Command> cmd = get_command(argv[1]);
        if (cmd) {
            cmd->handle(argv);
            return 0;
        }
    }

    free(tcp);
    return 0;
}
