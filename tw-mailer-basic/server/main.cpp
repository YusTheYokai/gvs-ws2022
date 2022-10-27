#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

#include "command.h"

#define BUFFER 1024

std::list<std::string> messages;
std::string OK = "OK\n";

void sendCommand(int clientSocketFD, std::vector<std::string>& message) {
    messages.push_back(message[1]);
    message.clear();
    message.push_back(OK);
}

void readCommand(int clientSocketFD, std::vector<std::string>& message) {
    if (messages.empty()) {
        // TODO: Use better exception
        throw std::runtime_error("No messages to read");
    }

    message.clear();
    message.push_back(OK);
    message.push_back(messages.front());
    messages.pop_front();
}

int main(int argc, char* argv[]) {
    std::map<std::string, Command> commands;
    commands.insert(std::pair<std::string, Command>("SEND", Command("Send", "", sendCommand)));
    commands.insert(std::pair<std::string, Command>("READ", Command("Read", "", readCommand)));

    int port;
    std::string directoryName;

    char c;

    while ((c = getopt(argc, argv, "")) != EOF) {
        switch (c) {
            case '?':
                std::cerr << "Unknown option" << std::endl;
                exit(1);
        }
    }

    if (optind < argc) {
        port = atoi(argv[optind++]);
        directoryName = argv[optind];
        // TODO: catch exception
    } else {
        std::cerr << "No port or spool directory name given" << std::endl;
        exit(1);
    }

    std::cout << "port: " << port << std::endl;
    std::cout << "directory name: " << directoryName << std::endl;

    int reuseValue = 1;
    int size;
    char buffer[BUFFER];
    socklen_t addressLength;
    struct sockaddr_in address;
    struct sockaddr_in clientAddress;

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        std::cerr << "Could not create socket" << std::endl;
        exit(1);
    }

    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuseValue, sizeof(int)) < 0) {
        std::cerr << "Could not set socket options" << std::endl;
        exit(1);
    }

    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEPORT, &reuseValue, sizeof(int)) < 0) {
        std::cerr << "Could not set socket options" << std::endl;
        exit(1);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketFD, (struct sockaddr*) &address, sizeof(address)) == -1) {
        std::cerr << "Bind has failed" << std::endl;
        exit(1);
    }

    if (listen(socketFD, 5) == -1) {
        std::cerr << "Could not listen" << std::endl;
        exit(1);
    }

    addressLength = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(socketFD, (struct sockaddr*) &clientAddress, &addressLength);
    if (clientSocketFD == -1) {
        std::cerr << "Could not accept" << std::endl;
        exit(1);
    }

    std::cout << "Connection established" << std::endl;

    std::vector<std::string> lines;
    do {
        size = recv(clientSocketFD, buffer, BUFFER - 1, 0);
        if (size == -1) {
            std::cerr << "Could not receive" << std::endl;
        } else if (size == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            buffer[size] = '\0';

            if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
                size -= 2;
            } else if (buffer[size - 1] == '\n') {
                size--;
            }

            buffer[size] = '\0';
            std::cout << "Received: " << buffer << std::endl;

            std::stringstream ss(buffer);
            std::string line;
            while (std::getline(ss, line, '\n')) {
                lines.push_back(line);
            }

            auto command = commands.at(lines[0]);
            try {
                command.getCommand()(clientSocketFD, lines);
            } catch (...) {
                std::cerr << "Command failed" << std::endl;
                exit(1);
            }
        }
    } while (lines[0] != "QUIT");
}
