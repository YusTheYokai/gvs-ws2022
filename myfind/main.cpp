#include <string>
#include <iostream>
#include <list>
#include <filesystem>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h> 

#define KEY 22376221
#define PERM 0660
#define MAX_DATA 255

namespace fs = std::filesystem;

typedef std::function<std::string(std::string)> stringMapper;

/**
 * Signal handler to collect processes once they have (not) found the file.
 */
void signalHandler(int sig);

/**
 * Parses arguments.
 * @param arc Argument count
 * @param argv Argument values
 * @param optionCounterR reference to recursive option counter
 * @param optionCounterI reference to ignore case option counter
 * @param path reference to path string
 * @param filenames reference to filename list
 */
void parseArguments(int argc, char* argv[],
                    unsigned short& optionCounterR, unsigned short& optionCounterI,
                    std::string& path, std::list<std::string>& filenames);

/**
 * Iterate over directory with certain path searching for filename.
 * @param path searchpath
 * @param filename name of the searched file
 * @param recursive boolean whether directories should be iterated throgh recursivly
 * @param stringMapper stringMapper function to adjust strings before comparison
 */
void iterateDirectory(const fs::path& path, std::string filename, bool recursive, stringMapper stringMapper);

/**
 * Send message, that file has been found, to message queue.
 * @param filePath path of the found file
 */
void sendFileFoundMessage(const fs::path& filePath);

typedef struct {
	long mType;
	char mText[MAX_DATA];
} message_t;

/**
 * 
 */
int main(int argc, char* argv[]) {
    // set signal handler for zombie processes
    signal(SIGCHLD, signalHandler);

    unsigned short optionCounterR = 0;
    unsigned short optionCounterI = 0;
    std::string path;
    std::list<std::string> filenames;

    parseArguments(argc, argv, optionCounterR, optionCounterI, path, filenames);

    // use lowercase mapper if i (ignore case) flag has been set
    stringMapper stringMapper = optionCounterI ?
            [] (std::string s) { std::transform(s.begin(), s.end(), s.begin(), ::tolower); return s; } :
            [] (std::string s) { return s; };

    int pid = getpid();

    int msgid;
    if ((msgid = msgget(KEY, PERM | IPC_CREAT | IPC_EXCL)) == -1) {
        std::cerr << "Was not able to create message queue" << std::endl;
        exit(4);
    }

    for (const auto& filename : filenames) {
        if (pid == getpid() && fork() == 0) {
            iterateDirectory(path, filename, optionCounterR, stringMapper);
            return 0;
        }
    }

    message_t message = { .mType = 0 };
    while (message.mType == 0) {
        if (msgrcv(msgid, &message, sizeof(message) - sizeof(long), 0 , 0) == -1) {
            // last message recieved
            break;
        } else {
            std::cout << message.mText << std::endl;
            message = { .mType = 0 };
        }
    }

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        std::cerr << "Could not remove message queue" << std::endl;
        exit(6);
    } else {
        std::cout << "Message queue removed" << std::endl;
    }

    return 0;
}

void signalHandler(int sig) {
    pid_t pid;
    int ret;

    while ((pid = waitpid(-1, &ret, WNOHANG)) > 0) {
        std::cout << "Child with pid " << std::to_string(pid) << " collected" << std::endl;
    }
}

void parseArguments(int argc, char* argv[],
                    unsigned short& optionCounterR, unsigned short& optionCounterI,
                    std::string& path, std::list<std::string>& filenames) {
    int c;

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
}

void iterateDirectory(const fs::path& searchPath, std::string filename, bool recursive, stringMapper stringMapper) {
    for (const auto& file : fs::directory_iterator(searchPath)) {
        const fs::path& filePath = file.path();

        // compare filenames after string mapper has been applied
        if (stringMapper(filePath.filename()).compare(stringMapper(filename)) == 0) {
            sendFileFoundMessage(filePath);
            return;
        } else if (recursive && fs::is_directory(file)) {
            iterateDirectory(file.path(), filename, recursive, stringMapper);
        }
    }
}

void sendFileFoundMessage(const fs::path& filePath) {
    int pid = getpid();
    int msgid;

    if ((msgid = msgget(KEY, PERM)) == -1) {
        std::cerr << pid << ": Could not access message queue" << std::endl;
        exit(10);
    }

    message_t message = { .mType = 1 };
    std::string text = std::to_string(pid) + ": " + filePath.filename().generic_string() + ": " + filePath.generic_string();
    text.copy(message.mText, MAX_DATA);

    if (msgsnd(msgid, &message, sizeof(message) - sizeof(long), 0) == -1) {
        std::cerr << pid << ": Could not send message" << std::endl;
        exit(11);
    }
}
