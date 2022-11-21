#include "clientCommand.h"

ClientCommand::ClientCommand(std::string name,
                             std::string description,
                             bool requiresAuthentication,
                             std::function<void(std::vector<std::string>&)> command,
                             std::function<void(std::vector<std::string>&)> callback) : Command(command) {
    this->name = name;
    this->description = description;
    this->requiresAuthentication = requiresAuthentication;
    this->callback = callback;
}

std::string ClientCommand::getName() {
    return name;
}

std::string ClientCommand::getDescription() {
    return description;
}

bool ClientCommand::getRequiresAuthentication() {
    return requiresAuthentication;
}

std::function<void(std::vector<std::string>&)> ClientCommand::getCallback() {
    return callback;
}
