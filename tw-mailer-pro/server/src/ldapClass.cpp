#include "ldapClass.h"

#include <ldap.h>
#include <stdexcept>

#include "ldapUtils.h"
#include "logger.h"

Ldap::Ldap(std::string uri, std::string base, ber_int_t scope, std::string filter) {
    this->uri = uri;
    this->base = base;
    this->scope = scope;
    this->filter = filter;
}

void Ldap::connect() {
    Logger::info("Connecting to LDAP server...");
    if (ldap_initialize(&ldap, uri.c_str()) != LDAP_SUCCESS) {
        std::string error = "Could not connect to LDAP server";
        Logger::error(error);
        throw std::runtime_error(error);
    }

    Logger::success("Connected to LDAP server");
}

void Ldap::setProtocolVersion(int ldapVersion) {
    Logger::info("Setting LDAP protocol version...");
    int rc;
    if ((rc = ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion)) != LDAP_SUCCESS) {
        ldap_unbind_ext_s(ldap, NULL, NULL);
        Logger::error(ldap_err2string(rc));
        throw std::runtime_error("Could not set LDAP protocol version");
    }

    Logger::success("Set LDAP protocol version");
}

void Ldap::startTls() {
    Logger::info("Starting TLS...");
    int rc;
    if ((rc = ldap_start_tls_s(ldap, NULL, NULL)) != LDAP_SUCCESS) {
        ldap_unbind_ext_s(ldap, NULL, NULL);
        Logger::error(ldap_err2string(rc));
        throw std::runtime_error("Could not start TLS");
    }

    Logger::success("Started TLS");
}

void Ldap::bind(std::string username, std::string usernameSuffix, std::string password) {
    Logger::info("Binding to LDAP server...");

    BerValue cred;
    cred.bv_val = (char*) password.c_str();
    cred.bv_len = password.length();

    int rc;
    if ((rc = ldap_sasl_bind_s(ldap, ("uid=" + username + usernameSuffix).c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, &servercredp)) != LDAP_SUCCESS) {
        ldap_unbind_ext_s(ldap, NULL, NULL);
        Logger::error(ldap_err2string(rc));
        throw std::runtime_error("Could not bind to LDAP server");
    }

    Logger::success("Bound to LDAP server");
}

bool Ldap::checkPassword(std::string username, std::string password) {
    Logger::info("Searching LDAP...");

    const char* attributes[] = { "uid", "cn", NULL };
    LDAPMessage* result;

    // LdapUtils::filter.c_str()
    int rc = ldap_search_ext_s(ldap, LdapUtils::base.c_str(), LdapUtils::scope, LdapUtils::filter.c_str(), (char **) attributes, 0, NULL, NULL, NULL, 500, &result);
    if (rc != LDAP_SUCCESS) {
        ldap_unbind_ext_s(ldap, NULL, NULL);
        Logger::error(ldap_err2string(rc));
        throw std::runtime_error("Could not search LDAP");
    }

    Logger::success("Searched LDAP");
    Logger::info("Total results: " + std::to_string(ldap_count_entries(ldap, result)));

    LDAPMessage* entry;
    for (entry = ldap_first_entry(ldap, result); entry != NULL; entry = ldap_next_entry(ldap, entry)) {
        Logger::info(ldap_get_dn(ldap, entry));

        // for (attribute = ldap_first_attribute(ld, entry, &ber); attribute != NULL; attribute = ldap_next_attribute(ld, entry, ber)) {
        //     if ((vals = ldap_get_values_len(ld, entry, attribute)) != NULL) {
        //         for (i = 0; i < ldap_count_values_len(vals); i++) {
        //             printf("\t%s: %s\n", attribute, vals[i]->bv_val);
        //         }
        //         ldap_value_free_len(vals);
        //     }
        //     /* free memory used to store the attribute */
        //     ldap_memfree(attribute);
        // }
        // /* free memory used to store the value structure */
        // if (ber != NULL)
        //     ber_free(ber, 0);

        // printf("\n");
    }

    ldap_msgfree(result);
    ldap_msgfree(entry);

    return true;
}
