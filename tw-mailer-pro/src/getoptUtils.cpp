#include "getoptUtils.h"

#include <stdexcept>
#include <getopt.h>

void GetoptUtils::parseArguments(std::string& str1, std::string& str2, int argc, char* argv[]) {
    char c;

    while ((c = getopt(argc, argv, "")) != EOF) {
        if (c == '?') {
            throw std::invalid_argument("Unknown option");
        }
    }

    if (optind < argc) {
        str1 = argv[optind++];
        str2 = argv[optind];
    } else {
        throw std::out_of_range("Not enough arguments");
    }
}
