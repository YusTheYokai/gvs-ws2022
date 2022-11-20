#ifndef LDAPCLASS
#define LDAPCLASS
#ifndef LDAP_CLASS
#define LDAP_CLASS

#include <ldap.h>
#include <string>

class Ldap {

    public:
        Ldap(std::string uri, std::string base, ber_int_t scope, std::string filter);

        void connect();
        void setProtocolVersion(int ldapVersion);
        void startTls();
        void bind(std::string username, std::string password);

    private:
        std::string uri;
        std::string base;
        ber_int_t scope;
        std::string filter;

        LDAP* ldap;
        LDAPMessage* result;
        LDAPMessage* error;
        BerElement* ber;
        char* attribute;
        BerValue** values;
        BerValue *servercredp;
};

#endif // LDAP


#endif // LDAPCLASS
