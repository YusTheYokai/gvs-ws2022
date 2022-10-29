#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <regex>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "command.h"
#include "messageUtils.h"
#include "usernameUtils.h"

#define BUFFER 1024

void sendCommand(int serverSocketFD, std::vector<std::string>& message) {
    std::string receiver;
    std::string subject;
    std::string content;
    
    std::cout << "Receiver: ";
    std::cin >> receiver;

    if (!UsernameUtils::usernameIsValid(receiver)) {
        throw std::invalid_argument("Invalid receiver username");
    }

    std::cout << "Subject: ";
    std::cin >> subject;
    std::cout << "Content: ";
    std::getline(std::cin >> std::ws, content);

    message.clear();
    message.push_back("SEND");
    message.push_back(UsernameUtils::username);
    message.push_back(receiver);
    message.push_back(subject);
    message.push_back(content);
    message.push_back(".");
}

void listCommand(int serverSocketFD, std::vector<std::string>& message) {
    message.clear();
    message.push_back("LIST");
    message.push_back(UsernameUtils::username);
}

// TODO: abstract read and delete
void readCommand(int serverSocketFD, std::vector<std::string>& message) {
    int messageNumber;

    std::cout << "Message number: ";
    std::cin >> messageNumber;

    message.clear();
    message.push_back("READ");
    message.push_back(UsernameUtils::username);
    message.push_back(std::to_string(messageNumber));
}

void deleteCommand(int serverSocketFD, std::vector<std::string>& message) {
    int messageNumber;

    std::cout << "Message number: ";
    std::cin >> messageNumber;

    message.clear();
    message.push_back("DEL");
    message.push_back(UsernameUtils::username);
    message.push_back(std::to_string(messageNumber));
}

void quitCommand(int serverSocketFD, std::vector<std::string>& message) {
    message.clear();
    message.push_back("QUIT");
}

int main(int argc, char* argv[]) {
    std::map<std::string, Command> commands;
    commands.insert(std::pair<std::string, Command>("SEND", Command("Send  ", "send a message",              sendCommand)));
    commands.insert(std::pair<std::string, Command>("LIST", Command("List  ", "list all messages of a user", listCommand)));
    commands.insert(std::pair<std::string, Command>("READ", Command("Read  ", "read a message",              readCommand)));
    commands.insert(std::pair<std::string, Command>("DEL ", Command("Delete", "deletes a message",           deleteCommand)));
    commands.insert(std::pair<std::string, Command>("QUIT", Command("Quit  ", "quit the client",             quitCommand)));

    std::string ip;
    int port;

    char c;

    while ((c = getopt(argc, argv, "")) != EOF) {
        switch (c) {
            case '?':
                std::cerr << "Unknown option" << std::endl;
                exit(1);
        }
    }

    if (optind < argc) {
        ip = argv[optind++];
        port = atoi(argv[optind]);
        // TODO: catch exception
    } else {
        std::cerr << "No ip or port given" << std::endl;
        exit(1);
    }

    std::cout << "ip: " << ip << std::endl;
    std::cout << "port: " << port << std::endl;

    int size;
    char buffer[BUFFER];
    struct sockaddr_in address;

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        std::cerr << "Could not create socket" << std::endl;
        exit(1);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton(ip.c_str(), &address.sin_addr);

    if (connect(socketFD, (struct sockaddr*) &address, sizeof(address)) == -1) {
        std::cerr << "Could not connect" << std::endl;
        exit(1);
    }

    std::cout << "Connection established" << std::endl;

    do {
        std::cout << "Username must match the regex " << UsernameUtils::regexString << std::endl;
        std::cout << "Username: ";
        std::cin >> UsernameUtils::username;
    } while (!UsernameUtils::usernameIsValid(UsernameUtils::username));

    std::cout << "Available commands:" << std::endl;
    for (auto command : commands) {
        std::cout << command.first << " - " << command.second.getName() << ": " << command.second.getDescription() << std::endl;
    }

    do {
        std::vector<std::string> lines;
        std::string selection;

        std::cout << "Please enter a command: ";
        std::cin >> selection;

        auto command = commands.at(selection);
        try {
            command.getCommand()(socketFD, lines);
            std::string message = MessageUtils::toString(lines);

            if (send(socketFD, message.c_str(), message.length(), 0) == -1) {
                std::cerr << "Could not send message" << std::endl;
                exit(1);
            }

            if (lines[0] == "QUIT") {
                exit(0);
            }

            size = recv(socketFD, buffer, BUFFER, 0);
            MessageUtils::validateMessage(size);
            MessageUtils::parseMessage(buffer, size, lines);

            for (auto line : lines) {
                std::cout << line << std::endl;
            }
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    } while (1);

    return 0;
}
