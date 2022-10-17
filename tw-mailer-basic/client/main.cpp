#include <iostream>
#include <string>

#include <unistd.h>

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
        exit(2);
    }

    std::cout << "ip: " << ip << std::endl;
    std::cout << "port: " << port << std::endl;

    std::cout << "Please enter your username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Available commands:" << std::endl;

    std::cout << " (1) SEND - send a message to the server" << std::endl;
    std::cout << " (2) LIST - list all of your messages" << std::endl;
    std::cout << " (3) READ - read one of your messages" << std::endl;
    std::cout << " (4) DEL  - remove a message" << std::endl;
    std::cout << " (5) QUIT - logout" << std::endl;

    return 0;
}
