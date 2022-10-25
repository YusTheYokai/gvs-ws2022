#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER 1024

int main(int argc, char* argv[]) {
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

    std::cout << "Please enter your username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Available commands:" << std::endl;

    std::cout << " (0) QUIT - logout" << std::endl;
    std::cout << " (1) SEND - send a message to the server" << std::endl;
    std::cout << " (2) LIST - list all of your messages" << std::endl;
    std::cout << " (3) READ - read one of your messages" << std::endl;
    std::cout << " (4) DEL  - remove a message" << std::endl;

    do {
        std::cout << "Please enter a command: ";
        int selection = 0;
        std::cin >> selection;
        // TODO: catch exception

        std::string message;
        if (selection == 0) {
            message = "QUIT";
        } else if (selection == 1) {
            message = "SEND";
        }

        if (send(socketFD, message.c_str(), message.size(), 0) == -1) {
            std::cerr << "Could not send message" << std::endl;
            exit(1);
        }

        size = recv(socketFD, buffer, BUFFER, 0);
        if (size == -1) {
            std::cerr << "Could not receive message" << std::endl;
            exit(1);
        } else if (size == 0) {
            std::cerr << "Server closed connection" << std::endl;
            exit(1);
        } else {
            buffer[size] = '\0';
            std::cout << "Received: " << buffer << std::endl;
        }
    } while (1);

    return 0;
}
