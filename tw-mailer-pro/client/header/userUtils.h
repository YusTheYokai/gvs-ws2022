#ifndef USERUTILS
#define USERUTILS

#include <string>
#include <regex>

/**
 * Utility class for user handling.
 */
class UserUtils {

    public:
        static std::string username;
        static std::string password;
        static bool authenticated;

        static std::string regexString;
        static std::regex regex;

        /**
         * Checks if a username is valid according to the regex.
         * @param username the username to check
         * @return true if the username is valid, false otherwise
         */
        static bool usernameIsValid(std::string username);
};

#endif // USERUTILS
