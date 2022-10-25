#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER 1024

int main(int argc, char* argv[]) {
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
            
            std::string message = "OK\n";
            if (send(clientSocketFD, message.c_str(), message.size(), 0) == -1) {
                std::cerr << "Could not send" << std::endl;
            }
        }
    } while (strcmp(buffer, "QUIT") != 0);
}