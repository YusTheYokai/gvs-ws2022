#include <string>
#include <regex>

class UsernameUtils {

    public:
        static std::string username;
        static std::string regexString;
        static std::regex regex;

        static bool usernameIsValid(std::string username);
};
