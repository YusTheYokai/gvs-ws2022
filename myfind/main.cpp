#include <string>
#include <iostream>
#include <list>
#include <filesystem>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 

namespace fs = std::filesystem;

typedef std::function<std::string(std::string)> stringMapper;

void iterateDirectory(const fs::path& path, std::string filename, bool recursive, stringMapper stringMapper);

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

    stringMapper stringMapper = optionCounterI ?
            [] (std::string s) { std::transform(s.begin(), s.end(), s.begin(), ::tolower); return s; } :
            [] (std::string s) { return s; };

    int pid = getpid();

    for (const auto& filename : filenames) {
        if (pid == getpid() && fork() == 0) {
            iterateDirectory(path, filename, optionCounterR, stringMapper);
        }
    }

    // unnötig?
    // pid_t childpid;
    // while ((childpid = waitpid(-1, NULL, WNOHANG))) {
    //     if (childpid == -1 && errno != EINTR) {
    //         break;
    //     }
    // }
}

void iterateDirectory(const fs::path& searchPath, std::string filename, bool recursive, stringMapper stringMapper) {
    for (const auto& file : fs::directory_iterator(searchPath)) {
        const fs::path& filePath = file.path();

        if (stringMapper(filePath.filename()).compare(stringMapper(filename)) == 0) {
            std::cout << getpid() << ": " << file.path().filename().generic_string() << ": " << file.path().generic_string() << std::endl;
        } else if (recursive && fs::is_directory(file)) {
            iterateDirectory(file.path(), filename, recursive, stringMapper);
        }
    }
}
