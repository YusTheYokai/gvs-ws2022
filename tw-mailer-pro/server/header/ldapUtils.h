#ifndef LDAPUTILS
#define LDAPUTILS

#include <string>

class LdapUtils {

    public:
        static std::string uri;
        static std::string base;
        static ber_int_t scope;
        static std::string filter;

        static std::string username;
        static std::string password;
};

#endif // LDAPUTILS
