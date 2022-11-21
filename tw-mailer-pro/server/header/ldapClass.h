#ifndef LDAPCLASS
#define LDAPCLASS

#include <ldap.h>
#include <string>

class Ldap {

    public:
        Ldap(std::string uri, std::string base, ber_int_t scope, std::string filter);

        void connect();
        void setProtocolVersion(int ldapVersion);
        void startTls();
        bool bind(std::string username, std::string usernameSuffix, std::string password);

    private:
        std::string uri;
        std::string base;
        ber_int_t scope;
        std::string filter;

        LDAP* ldap;
};

#endif // LDAPCLASS
