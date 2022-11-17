#include <algorithm>
#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "command.h"
#include "getoptUtils.h"
#include "logger.h"
#include "messageUtils.h"

namespace fs = std::filesystem;

#define BUFFER 1024

std::string OK = "OK";
std::string ERR = "ERR";

int clientCommunication(int clientSocketFD, std::map<std::string, Command>& commands);

void sendCommand(std::string directoryName, std::vector<std::string>& message) {
    // 0 = command, 1 = sender, 2 = receiver, 3 = subject, 4 = content
    fs::create_directory(directoryName + "/" + message[2]);
    std::string fileName = directoryName + "/" + message[2] + "/" + message[3];

    std::ofstream file(fileName);
    file << message[1] << std::endl << message[3] << std::endl << message[4];
    file.close();

    message.clear();
    message.push_back(OK);
}

void listCommand(std::string directoryName, std::vector<std::string>& message) {
    int count = 0;

    message.clear();
    message.push_back("");
    
    try {
        fs::directory_iterator dir(directoryName + "/" + message[1]);
        for (auto file : dir) {
            message.push_back(file.path().filename());
            count++;
        }
    } catch (fs::filesystem_error& e) {
        // noop
    }
    message[0] = std::to_string(count);
}

void readCommand(std::string directoryName, std::vector<std::string>& message) {
    if (!fs::is_directory(directoryName + "/" + message[1])) {
        message.clear();
        message.push_back(ERR);
        return;
    }

    fs::path filePath = "";

    int count = 0;
    fs::directory_iterator dir(directoryName + "/" + message[1]);
    for (auto file : dir) {
        if (count == std::stoi(message[2])) {
            filePath = file.path();
        }
        count++;
    }

    message.clear();
    if (filePath == "") {
        message.push_back(ERR);
        return;
    }

    std::ifstream file(filePath);
    std::string line;
    message.push_back(OK);
    while (std::getline(file, line)) {
        message.push_back(line);
    }
    file.close();
}

void deleteCommand(std::string directoryName, std::vector<std::string>& message) {
    if (!fs::is_directory(directoryName + "/" + message[1])) {
        message.clear();
        message.push_back(ERR);
        return;
    }

    fs::path filePath = "";

    int count = 0;
    fs::directory_iterator dir(directoryName + "/" + message[1]);
    for (auto file : dir) {
        if (count == std::stoi(message[2])) {
            filePath = file.path();
        }
        count++;
    }

    if (filePath == "") {
        message.clear();
        message.push_back(ERR);
        return;
    }

    if (!fs::remove(filePath)) {
        message.clear();
        message.push_back(ERR);
        return;
    }

    message.clear();
    message.push_back(OK);
}

void quitCommand(std::vector<std::string>& message) {
    Logger::info("Client has disconnected");
    throw std::runtime_error("quit");
}

int main(int argc, char* argv[]) {
    signal(SIGINT, [](int flag) { exit(0); });

    std::string port;
    std::string directoryName;

    std::map<std::string, Command> commands;
    commands.insert(std::pair<std::string, Command>("SEND", Command("", "",
            [&directoryName] (std::vector<std::string>& message) { sendCommand(directoryName, message); })));
    commands.insert(std::pair<std::string, Command>("LIST", Command("", "",
            [&directoryName] (std::vector<std::string>& message) { listCommand(directoryName, message); })));
    commands.insert(std::pair<std::string, Command>("READ", Command("", "",
            [&directoryName] (std::vector<std::string>& message) { readCommand(directoryName, message); })));
    commands.insert(std::pair<std::string, Command>("DEL" , Command("", "",
            [&directoryName] (std::vector<std::string>& message) { deleteCommand(directoryName, message); })));
    commands.insert(std::pair<std::string, Command>("QUIT", Command("", "", quitCommand)));

    try {
        GetoptUtils::parseArguments(port, directoryName, argc, argv);
    } catch (std::invalid_argument& e) {
        Logger::error(e.what());
        exit(1);
    } catch (std::out_of_range& e) {
        Logger::error("port or spool directory name missing");
        exit(1);
    }

    fs::create_directory(directoryName);

    int reuseValue = 1;
    socklen_t addressLength = sizeof(struct sockaddr_in);
    struct sockaddr_in address;
    struct sockaddr_in clientAddress;

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        Logger::error("Could not create socket");
        exit(1);
    }

    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuseValue, sizeof(int)) < 0 ||
            setsockopt(socketFD, SOL_SOCKET, SO_REUSEPORT, &reuseValue, sizeof(int)) < 0) {
        Logger::error("Could not set socket options");
        exit(1);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(std::stoi(port));
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketFD, (struct sockaddr*) &address, sizeof(address)) == -1) {
        Logger::error("Bind has failed");
        exit(1);
    }

    if (listen(socketFD, 5) == -1) {
        Logger::error("Could not listen");
        exit(1);
    }

    Logger::info("Accepting connections on port " + port + "...");

    std::vector<std::thread> threads;

    while (1) {
        int clientSocketFD = accept(socketFD, (struct sockaddr*) &clientAddress, &addressLength);
        std::cout << "Hier" << std::endl;
        threads.push_back(std::thread(clientCommunication, clientSocketFD, std::ref(commands)));
        std::cout << "Danach" << std::endl;
    }
}

int clientCommunication(int clientSocketFD, std::map<std::string, Command>& commands) {
    std::cout << "bin drin" << std::endl;
    if (clientSocketFD == -1) {
        Logger::error("Could not accept");
        exit(1);
    }

    Logger::success("Connection established");

    int size;
    char buffer[BUFFER];
    std::vector<std::string> lines;

    while (1) {
        size = recv(clientSocketFD, buffer, BUFFER - 1, 0);
        MessageUtils::validateMessage(size);
        MessageUtils::parseMessage(buffer, size, lines);

        try {
            auto command = commands.at(lines[0]);
            command.getCommand()(lines);
        } catch (std::out_of_range& e) {
            lines.clear();
            lines.push_back(ERR);
            lines.push_back("Unknown command");
        } catch (std::runtime_error& e) {
            break;
        }

        std::string response = MessageUtils::toString(lines);

        if (send(clientSocketFD, response.c_str(), response.size(), 0) == -1) {
            Logger::error("Could not send");
            return 1;
        }
    }
    return 0;
}
