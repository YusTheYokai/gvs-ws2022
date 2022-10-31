#ifndef GETOPTUTILS
#define GETOPTUTILS

#include <string>

/**
 * Utility class for getopt handling.
 */
class GetoptUtils {

    public:
        static void parseArguments(std::string& str1, std::string& str2, int argc, char* argv[]);
};

#endif // GETOPTUTILS
