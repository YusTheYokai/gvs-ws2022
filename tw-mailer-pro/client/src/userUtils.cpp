#include "userUtils.h"

std::string UserUtils::username = "";
std::string UserUtils::password = "";
bool UserUtils::authenticated = false;
std::string UserUtils::regexString = "^[a-z0-9]{3,8}$";
std::regex UserUtils::regex = std::regex(UserUtils::regexString);

bool UserUtils::usernameIsValid(std::string username) {
    return std::regex_match(username, regex);
}
