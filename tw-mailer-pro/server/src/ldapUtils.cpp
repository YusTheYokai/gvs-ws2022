#include "ldapUtils.h"

#include <ldap.h>

std::string LdapUtils::uri = "ldap://ldap.technikum-wien.at:389";
std::string LdapUtils::base = "dc=technikum-wien,dc=at";
ber_int_t LdapUtils::scope = LDAP_SCOPE_SUBTREE;
std::string LdapUtils::filter = "(uid=if21b*)";
std::string LdapUtils::usernameSuffix = ",ou=people,dc=technikum-wien,dc=at";
