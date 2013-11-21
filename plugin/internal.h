/*
 * Internal prototypes and structures for the kadmind password update plugin.
 *
 * Written by Russ Allbery <eagle@eyrie.org>
 * Based on code developed by Derrick Brashear and Ken Hornstein of Sine
 *     Nomine Associates, on behalf of Stanford University.
 * Copyright 2006, 2007, 2010, 2013
 *     The Board of Trustees of the Leland Stanford Junior University
 *
 * See LICENSE for licensing terms.
 */

#ifndef PLUGIN_INTERNAL_H
#define PLUGIN_INTERNAL_H 1

#include <config.h>
#include <portable/krb5.h>
#include <portable/macros.h>
#include <portable/stdbool.h>

#ifdef HAVE_KRB5_KADM5_HOOK_PLUGIN
# include <krb5/kadm5_hook_plugin.h>
#else
typedef struct kadm5_hook_modinfo_st kadm5_hook_modinfo;
#endif

/*
 * Local configuration information for the module.  This contains all the
 * parameters that are read from the krb5-sync sub-section of the appdefaults
 * section when the module is initialized.
 *
 * MIT Kerberos uses this type as an abstract data type for any data that a
 * kadmin hook needs to carry.  Reuse it since then we get type checking for
 * at least the MIT plugin.
 */
struct kadm5_hook_modinfo_st {
    char *ad_admin_server;
    char *ad_base_instance;
    char *ad_instances;
    char *ad_keytab;
    char *ad_ldap_base;
    char *ad_principal;
    bool ad_queue_only;
    char *ad_realm;
    char *queue_dir;
};

BEGIN_DECLS

/* Default to a hidden visibility for all internal functions. */
#pragma GCC visibility push(hidden)

/* Initialize the plugin and set up configuration. */
krb5_error_code sync_init(krb5_context, kadm5_hook_modinfo **);

/* Free the internal plugin state. */
void sync_close(kadm5_hook_modinfo *);

/* Handle a password change. */
krb5_error_code sync_chpass(kadm5_hook_modinfo *, krb5_context,
                            krb5_principal, const char *password);

/* Handle an account status change. */
krb5_error_code sync_status(kadm5_hook_modinfo *, krb5_context,
                            krb5_principal, bool enabled);

/* Password changing in Active Directory. */
krb5_error_code sync_ad_chpass(kadm5_hook_modinfo *, krb5_context,
                               krb5_principal, const char *password);

/* Account status update in Active Directory. */
krb5_error_code sync_ad_status(kadm5_hook_modinfo *, krb5_context,
                               krb5_principal, bool enabled);

/*
 * Returns true if the principal has only one component and two-component
 * principal with instance added exists in the Kerberos database.
 */
int sync_instance_exists(kadm5_hook_modinfo *, krb5_context, krb5_principal,
                         const char *instance);

/* Returns true if there is a queue conflict for this operation. */
int sync_queue_conflict(kadm5_hook_modinfo *, krb5_context, krb5_principal,
                        const char *domain, const char *operation);

/* Writes an operation to the queue. */
krb5_error_code sync_queue_write(kadm5_hook_modinfo *, krb5_context,
                                 krb5_principal, const char *domain,
                                 const char *operation, const char *password);

/*
 * Obtain configuration settings from krb5.conf.  These are wrappers around
 * the krb5_appdefault_* APIs that handle setting the section name, obtaining
 * the local default realm and using it to find settings, and doing any
 * necessary conversion.
 */
void sync_config_boolean(krb5_context, const char *, bool *)
    __attribute__((__nonnull__));
void sync_config_string(krb5_context, const char *, char **)
    __attribute__((__nonnull__));

/*
 * Store a configuration, generic, or system error in the Kerberos context,
 * appending the strerror results to the message in the _system case and the
 * LDAP error string in the _ldap case.  Returns the error code set.
 */
krb5_error_code sync_error_config(krb5_context, const char *format, ...)
    __attribute__((__nonnull__, __format__(printf, 2, 3)));
krb5_error_code sync_error_generic(krb5_context, const char *format, ...)
    __attribute__((__nonnull__, __format__(printf, 2, 3)));
krb5_error_code sync_error_ldap(krb5_context, int, const char *format, ...)
    __attribute__((__nonnull__, __format__(printf, 3, 4)));
krb5_error_code sync_error_system(krb5_context, const char *format, ...)
    __attribute__((__nonnull__, __format__(printf, 2, 3)));

/* Undo default visibility change. */
#pragma GCC visibility pop

END_DECLS

#endif /* !PLUGIN_INTERNAL_H */
