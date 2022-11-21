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
        void bind(std::string username, std::string usernameSuffix, std::string password);
        bool checkPassword(std::string username, std::string password);

    private:
        std::string uri;
        std::string base;
        ber_int_t scope;
        std::string filter;

        LDAP* ldap;
        BerElement* ber;
        char* attribute;
        BerValue** values;
        BerValue *servercredp;
};

#endif // LDAPCLASS
