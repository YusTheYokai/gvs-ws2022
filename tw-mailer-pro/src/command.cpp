#include "command.h"

Command::Command(std::function<void(std::vector<std::string>&)> command) {
    this->command = command;
}

std::function<void(std::vector<std::string>&)> Command::getCommand() {
    return command;
}
