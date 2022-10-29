#include "usernameUtils.h"

std::string UsernameUtils::regexString = "^[a-z0-9]{3,8}$";
std::regex UsernameUtils::regex = std::regex(UsernameUtils::regexString);

bool UsernameUtils::usernameIsValid(std::string username) {
    return std::regex_match(username, regex);
}
