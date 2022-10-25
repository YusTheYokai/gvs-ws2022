#include "command.h"

Command::Command(std::string name, std::string description, std::function<bool(int, char[])> command) {
    this->name = name;
    this->description = description;
    this->command = command;
}
