#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <memory>
#include <iostream>

class Command {
public:
    std::string name;
    
    virtual bool handle(char** argv) {
        std::cerr << "Base Command cannot handle actions directly.\n";
        return false;
    }

    virtual ~Command() = default;
};

#endif // COMMAND_H