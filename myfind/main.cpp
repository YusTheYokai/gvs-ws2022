#include <unistd.h>
#include <string>
#include <iostream>
#include <list>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    int c;
    unsigned short optionCounterR = 0;
    unsigned short optionCounterI = 0;
    std::string path;
    std::list<std::string> filenames;

    while ((c = getopt(argc, argv, "Ri")) != EOF) {
        switch (c) {
        case '?':
            std::cerr << "Unknown option" << std::endl;
            exit(1);
        case 'R':
            optionCounterR++;
            break;
        case 'i':
            optionCounterI++;
            break;
        }
    }

    if ((optionCounterR > 1) || (optionCounterI > 1)) {
        std::cerr << "Options used mutliple times" << std::endl;
        exit(1);
    }

    if (optind < argc) {
        path = argv[optind++];
        while (optind < argc) {
            filenames.push_back(argv[optind++]);
        }
    } else {
        std::cerr << "No searchpath or filename given" << std::endl;
        exit(2);
    }

    if (filenames.empty()) {
        std::cerr << "No filename given" << std::endl;
        exit(2);
    }

    if (!fs::is_directory(path)) {
        std::cerr << "Searchpath is not a directory" << std::endl;
        exit(3);
    }

    InputIterator 
    for (const auto& file : make_directory_range(optionCounterR, path)) {
        if (std::find(std::begin(filenames), std::end(filenames), file.path().filename()) != end(filenames)) {
            std::cout << "File " << file.path().filename() << " found at " << file.path() << std::endl;
        }
    }
}
