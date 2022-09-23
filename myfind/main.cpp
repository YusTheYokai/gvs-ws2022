#include <unistd.h>
#include <string>
#include <iostream>
#include <list>
#include <filesystem>
#include <iterator>
#include <algorithm>
#include <vector>

namespace fs = std::filesystem;

typedef std::function<int(std::string, std::string)> Comparator;

void iterateDirectory(const fs::path& path, std::string filename, bool recursive, Comparator comparator);

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

    for (const auto& filename : filenames) {
        // TODO: start process
        iterateDirectory(path, filename, optionCounterR, comparator);
    }
}

void iterateDirectory(const fs::path& path, std::string filename, bool recursive, Comparator comparator) {
    for (const auto& file : fs::directory_iterator(path)) {
        // TODO: case insensitive search if i flag is used
        if (comparator(file.path().filename(), filename) == 0) {
            std::cout << "File " << file.path().filename() << " found at " << file.path() << std::endl;
        } else if (recursive && fs::is_directory(file)) {
            iterateDirectory(file.path(), filename, recursive, comparator);
        }
    }
}
