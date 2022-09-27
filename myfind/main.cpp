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

/**
 * Signal handler to collect processes once they have (not) found the file.
 */
void signalHandler(int sig);

/**
 * Iterate over directory with certain path searching for filename.
 * @param path searchpath
 * @param filename name of the searched file
 * @param recursive boolean whether directories should be iterated throgh recursivly
 * @param stringMapper stringMapper function to adjust strings before comparison
 */
void iterateDirectory(const fs::path& path, std::string filename, bool recursive, stringMapper stringMapper);

int main(int argc, char *argv[]) {
    // set signal handler for zombie processes
    signal(SIGCHLD, signalHandler);

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

    // use lowercase mapper if i (ignore case) flag has been set
    stringMapper stringMapper = optionCounterI ?
            [] (std::string s) { std::transform(s.begin(), s.end(), s.begin(), ::tolower); return s; } :
            [] (std::string s) { return s; };

    int pid = getpid();

    for (const auto& filename : filenames) {
        if (pid == getpid() && fork() == 0) {
            iterateDirectory(path, filename, optionCounterR, stringMapper);
            return 0;
        }
    }
}

void signalHandler(int sig) {
    pid_t pid;
    int ret;

    while ((pid = waitpid(-1, &ret, WNOHANG)) > 0) {
        std::cout << "Child with pid " << std::to_string(pid) << " collected" << std::endl;
    }
}

void iterateDirectory(const fs::path& searchPath, std::string filename, bool recursive, stringMapper stringMapper) {
    for (const auto& file : fs::directory_iterator(searchPath)) {
        const fs::path& filePath = file.path();

        // compare filenames after string mapper has been applied
        if (stringMapper(filePath.filename()).compare(stringMapper(filename)) == 0) {
            std::cout << getpid() << ": " << file.path().filename().generic_string() << ": " << file.path().generic_string() << std::endl;
            return;
        } else if (recursive && fs::is_directory(file)) {
            iterateDirectory(file.path(), filename, recursive, stringMapper);
        }
    }
}
