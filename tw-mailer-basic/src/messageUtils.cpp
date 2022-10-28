#include "messageUtils.h"

#include <iostream>
#include <sstream>

void MessageUtils::validateMessage(int size) {
    if (size == -1) {
        std::cerr << "Could not receive" << std::endl;
        exit(1);
    } else if (size == 0) {
        std::cout << "Disconnected" << std::endl;
        exit(1);
    }
}

void MessageUtils::parseMessage(char buffer[], int size, std::vector<std::string>& message) {
    buffer[size] = '\0';
    if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
        size -= 2;
    } else if (buffer[size - 1] == '\n') {
        size--;
    }
    buffer[size] = '\0';

    std::stringstream ss(buffer);
    std::string line;

    message.clear();
    while (std::getline(ss, line, '\n')) {
        message.push_back(line);
    }
}

std::string MessageUtils::toString(std::vector<std::string> message) {
    std::string s;
    for (auto line : message) {
        s += line + "\n";
    }
    return s;
}
