#include "command.h"

Command::Command(std::string name, std::string description, std::function<bool(int, char[])> command) {
    this->name = name;
    this->description = description;
    this->command = command;
}

std::string Command::getName() {
    return name;
}

std::string Command::getDescription() {
    return description;
}

std::function<bool(int, char[])> Command::getCommand() {
    return command;
}
