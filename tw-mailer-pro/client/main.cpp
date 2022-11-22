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

#include "clientCommand.h"
#include "getoptUtils.h"
#include "logger.h"
#include "messageUtils.h"
#include "userUtils.h"

#define BUFFER 1024

std::string OK = "OK";
std::string ERR = "ERR";

/**
 * Connect to the server.
 * @param ip the server ip
 * @param port the server port
 * @throws invalid_argument if the port is invalid
 */
int connectToServer(std::string ip, std::string port);

/**
 * Prints all commands available to the user right now.
 * @param commands the commands to print
 */
void printAvailableCommands(std::map<std::string, ClientCommand> commands);

void loginCommand(std::vector<std::string>& message) {
    std::string password;

    do {
        Logger::plain("Username must match the regex " + UserUtils::regexString);
        std::cout << "Username: ";
        std::cin >> UserUtils::username;
        password = getpass("Password: ");
    } while (!UserUtils::usernameIsValid(UserUtils::username));

    message.clear();
    message.push_back("LOGIN");
    message.push_back(UserUtils::username);
    message.push_back(password);
}

void loginCallback(std::vector<std::string>& message, std::map<std::string, ClientCommand> commands) {
    // 0 = OK/ERR, 1 = message (if 0 == ERR)
    if (message[0].compare(OK) == 0) {
        Logger::success("Logged in successfully");
        UserUtils::authenticated = true;
        printAvailableCommands(commands);
    } else {
        Logger::error(message[1]);
    }
}

/**
 * Send a mail to someone.
 */
void sendCommand(std::vector<std::string>& message) {
    std::string receiver;
    std::string subject;
    std::string content;

    std::cout << "Receiver (this can be multiple, seperated by comma):" << std::endl << ">> ";
    std::cin >> receiver;

    std::stringstream ss(receiver);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!UserUtils::usernameIsValid(token)) {
            throw std::invalid_argument("Invalid receiver username: " + token);
        }
    }

    std::cout << "Subject:" << std::endl << ">> ";
    std::getline(std::cin >> std::ws, subject);
    std::cout << "Content (this can be multi line, end with '\\'):" << std::endl << ">> ";
    std::getline(std::cin, content, '\\');

    message.clear();
    message.push_back("SEND");
    message.push_back(UserUtils::username);
    message.push_back(receiver);
    message.push_back(subject);
    message.push_back(content);
    message.push_back(".");
}

/**
 * List all mails.
 */
void listCommand(std::vector<std::string>& message) {
    message.clear();
    message.push_back("LIST");
    message.push_back(UserUtils::username);
}

/**
 * Callback for list command. Prints all mails.
 */
void listCallback(std::vector<std::string>& message) {
    for (auto line : message) {
        Logger::plain(line);
    }
}

/**
 * Access a mail.
 * @param command the command to execute
 */
void accessCommand(std::string command, std::vector<std::string>& message) {
    int messageNumber;

    std::cout << "Message number:" << std::endl << ">> " ;
    std::cin >> messageNumber;

    message.clear();
    message.push_back(command);
    message.push_back(UserUtils::username);
    message.push_back(std::to_string(messageNumber));
}

/**
 * Read a mail.
 */
void readCommand(std::vector<std::string>& message) {
    accessCommand("READ", message);
}

void readCallback(std::vector<std::string>& message) {
    // 0 = code, 1 = sender, 2 = subject, 3+ = content
    Logger::plain("| From: " + message[1]);
    Logger::plain("| Subject: " + message[2] + "\n|");

    std::for_each(message.begin() + 3, message.end(), [] (std::string line) {
        Logger::plain("| " + line);
    });
}

/**
 * Delete a mail.
 */
void deleteCommand(std::vector<std::string>& message) {
    accessCommand("DEL", message);
}

/**
 * Quit the client.
 */
void quitCommand(std::vector<std::string>& message) {
    message.clear();
    message.push_back("QUIT");
}

void defaultCallback(std::vector<std::string>& message) {
    if (message[0].compare(OK) == 0) {
        Logger::success("Success");
    } else {
        Logger::error("Error");
    }
}

int main(int argc, char* argv[]) {
    std::map<std::string, ClientCommand> commands;
    commands.insert(std::pair<std::string, ClientCommand>("LOGIN", ClientCommand("Login ", "Log in via LDAP",    false, loginCommand,  [&commands] (std::vector<std::string>& message) { loginCallback(message, commands); })));
    commands.insert(std::pair<std::string, ClientCommand>("SEND" , ClientCommand(" Send  ", "Send a message",     true,  sendCommand,   defaultCallback)));
    commands.insert(std::pair<std::string, ClientCommand>("LIST" , ClientCommand(" List  ", "List your messages", true,  listCommand,   listCallback)));
    commands.insert(std::pair<std::string, ClientCommand>("READ" , ClientCommand(" Read  ", "Read a message",     true,  readCommand,   readCallback)));
    commands.insert(std::pair<std::string, ClientCommand>("DEL"  , ClientCommand("  Delete", "Delete a message",   true,  deleteCommand, defaultCallback)));
    commands.insert(std::pair<std::string, ClientCommand>("QUIT" , ClientCommand(" Quit  ", "Quit the client",    false, quitCommand,   NULL)));

    std::string ip;
    std::string port;

    try {
        GetoptUtils::parseArguments(ip, port, argc, argv);
    } catch (std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    } catch (std::out_of_range& e) {
        Logger::error("ip or port missing");
        exit(1);
    }

    int socketFD;
    int size;
    char buffer[BUFFER];

    try {
        socketFD = connectToServer(ip, port);
    } catch (std::invalid_argument& e) {
        Logger::error("invalid port");
        exit(1);
    } catch (std::runtime_error& e) {
        Logger::error(e.what());
        exit(1);
    }

    Logger::success("Connection established");
    printAvailableCommands(commands);

    do {
        std::vector<std::string> lines;
        std::string selection;

        std::cout << "Please enter a command:" << std::endl << ">> ";
        std::cin >> selection;

        try {
            auto command = commands.at(selection);
            if (command.getRequiresAuthentication() && !UserUtils::authenticated) {
                throw std::runtime_error("You need to be authenticated to use this command");
            }

            command.getCommand()(lines);
            std::string message = MessageUtils::toString(lines);

            if (send(socketFD, message.c_str(), message.length(), 0) == -1) {
                Logger::error("Could not send message");
                exit(1);
            }

            if (lines[0] == "QUIT") {
                exit(0);
            }

            size = recv(socketFD, buffer, BUFFER, 0);

            if (!MessageUtils::messageIsValid(size)) {
                Logger::error("Server has closed the connection unexpectedly");
                exit(1);
            }

            MessageUtils::parseMessage(buffer, size, lines);
            command.getCallback()(lines);
        } catch (std::out_of_range& e) {
            Logger::error("Invalid command");
        } catch (std::exception& e) {
            Logger::error(e.what());
        }
    } while (1);
}

int connectToServer(std::string ip, std::string port) {
    struct sockaddr_in address;
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFD == -1) {
        throw std::runtime_error("Could not create socket");
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(std::stoi(port));
    inet_aton(ip.c_str(), &address.sin_addr);

    if (connect(socketFD, (struct sockaddr*) &address, sizeof(address)) == -1) {
        throw std::runtime_error("Could not connect");
    }

    return socketFD;
}

void printAvailableCommands(std::map<std::string, ClientCommand> commands) {
    Logger::plain("Available commands:");
    for (auto command : commands) {
        if (!command.second.getRequiresAuthentication() || UserUtils::authenticated) {
            Logger::plain(command.first + " - " + command.second.getName() + ": " + command.second.getDescription());
        }
    }
}
