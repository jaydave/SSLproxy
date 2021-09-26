/*-
 * SSLsplit - transparent SSL/TLS interception
 * https://www.roe.ch/SSLsplit
 *
 * Copyright (c) 2009-2019, Daniel Roethlisberger <daniel@roe.ch>.
 * Copyright (c) 2017-2021, Soner Tari <sonertari@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "opts.h"
#include "filter.h"

#include "sys.h"
#include "log.h"
#include "defaults.h"
#include "util.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>

#ifndef OPENSSL_NO_DH
#include <openssl/dh.h>
#endif /* !OPENSSL_NO_DH */
#include <openssl/x509.h>

/*
 * Temporary struct used while configuring proxyspec.
 * These vars are used while configuring proxyspecs,
 * and freed right after they are used, not in proxyspec_free().
 */
typedef struct spec_addrs {
	int af;
	char *addr;
	char *divert_addr;
	char *target_addr;
} spec_addrs_t;

/*
 * The topmost caller must exit with EXIT_FAILURE.
 * Returning -1 instead of calling exit() is necessary for reporting the 
 * include file the error has occurred in.
 */
int
oom_return(const char *argv0)
{
	fprintf(stderr, "%s: out of memory\n", argv0);
	return -1;
}

void *
oom_return_null(const char *argv0)
{
	fprintf(stderr, "%s: out of memory\n", argv0);
	return NULL;
}

int
oom_return_na()
{
	fprintf(stderr, "Out of memory\n");
	return -1;
}

void *
oom_return_na_null()
{
	fprintf(stderr, "Out of memory\n");
	return NULL;
}

/*
 * Load a cert/chain/key combo from a single PEM file.
 * Returns NULL on failure.
 */
cert_t *
opts_load_cert_chain_key(const char *filename)
{
	cert_t *cert;

	cert = cert_new_load(filename);
	if (!cert) {
		log_err_level_printf(LOG_CRIT, "Failed to load cert and key from PEM file "
		                "'%s'\n", filename);
		return NULL;
	}
	if (X509_check_private_key(cert->crt, cert->key) != 1) {
		log_err_level_printf(LOG_CRIT, "Cert does not match key in PEM file "
		                "'%s':\n", filename);
		ERR_print_errors_fp(stderr);
		return NULL;
	}

#ifdef DEBUG_CERTIFICATE
	log_dbg_printf("Loaded '%s':\n", filename);
	log_dbg_print_free(ssl_x509_to_str(cert->crt));
	log_dbg_print_free(ssl_x509_to_pem(cert->crt));
#endif /* DEBUG_CERTIFICATE */
	return cert;
}

opts_t *
opts_new(void)
{
	opts_t *opts;

	opts = malloc(sizeof(opts_t));
	if (!opts)
		return oom_return_na_null();
	memset(opts, 0, sizeof(opts_t));

	opts->divert = 1;
	opts->sslcomp = 1;
	opts->chain = sk_X509_new_null();
	opts->sslmethod = SSLv23_method;
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER >= 0x20702000L)
	opts->minsslversion = TLS1_VERSION;
#ifdef HAVE_TLSV13
	opts->maxsslversion = TLS1_3_VERSION;
#else /* !HAVE_TLSV13 */
	opts->maxsslversion = TLS1_2_VERSION;
#endif /* !HAVE_TLSV13 */
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	opts->remove_http_referer = 1;
	opts->verify_peer = 1;
#ifndef WITHOUT_USERAUTH
	opts->user_timeout = 300;
#endif /* !WITHOUT_USERAUTH */
	opts->max_http_header_size = 8192;
	return opts;
}

global_t *
global_new(void)
{
	global_t *global;

	global = malloc(sizeof(global_t));
	if (!global)
		return oom_return_na_null();
	memset(global, 0, sizeof(global_t));

	global->leafkey_rsabits = DFLT_LEAFKEY_RSABITS;
	global->conn_idle_timeout = 120;
	global->expired_conn_check_period = 10;
	global->stats_period = 1;

	global->opts = opts_new();
	if (!global->opts)
		return NULL;
	global->opts->global = global;
	return global;
}

#ifndef WITHOUT_USERAUTH
static void
free_userlist(userlist_t *ul)
{
	while (ul) {
		userlist_t *next = ul->next;
		free(ul->user);
		free(ul);
		ul = next;
	}
}
#endif /* !WITHOUT_USERAUTH */

void
opts_free(opts_t *opts)
{
	if (opts->chain) {
		sk_X509_pop_free(opts->chain, X509_free);
	}
	if (opts->clientcrt) {
		X509_free(opts->clientcrt);
	}
	if (opts->clientkey) {
		EVP_PKEY_free(opts->clientkey);
	}
	if (opts->cacrt) {
		X509_free(opts->cacrt);
	}
	if (opts->cakey) {
		EVP_PKEY_free(opts->cakey);
	}
#ifndef OPENSSL_NO_DH
	if (opts->dh) {
		DH_free(opts->dh);
	}
#endif /* !OPENSSL_NO_DH */
#ifndef OPENSSL_NO_ECDH
	if (opts->ecdhcurve) {
		free(opts->ecdhcurve);
	}
#endif /* !OPENSSL_NO_ECDH */
	if (opts->ciphers) {
		free(opts->ciphers);
	}
	if (opts->ciphersuites) {
		free(opts->ciphersuites);
	}
#ifndef WITHOUT_USERAUTH
	if (opts->user_auth_url) {
		free(opts->user_auth_url);
	}
	free_userlist(opts->divertusers);
	free_userlist(opts->passusers);
#endif /* !WITHOUT_USERAUTH */

	filter_macro_free(opts);

	// No need to call filter_rules_free() here, filter rules are freed during startup
	filter_rules_free(opts);
	filter_free(opts);

	memset(opts, 0, sizeof(opts_t));
	free(opts);
}

static void
spec_addrs_free(spec_addrs_t *spec_addrs)
{
	if (spec_addrs->addr)
		free(spec_addrs->addr);
	if (spec_addrs->divert_addr)
		free(spec_addrs->divert_addr);
	if (spec_addrs->target_addr)
		free(spec_addrs->target_addr);
	memset(spec_addrs, 0, sizeof(spec_addrs_t));
	free(spec_addrs);
}

/*
 * Clear and free a proxy spec.
 */
void
proxyspec_free(proxyspec_t *spec)
{
	if (spec->opts)
		opts_free(spec->opts);
	if (spec->natengine)
		free(spec->natengine);
	memset(spec, 0, sizeof(proxyspec_t));
	free(spec);
}

/*
 * Clear and free all proxy specs.
 */
void
global_proxyspec_free(proxyspec_t *spec)
{
	do {
		proxyspec_t *next = spec->next;
		proxyspec_free(spec);
		spec = next;
	} while (spec);
}

void
tmp_global_opts_free(tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts->cacrt_str) {
		free(tmp_global_opts->cacrt_str);
		tmp_global_opts->cacrt_str = NULL;
	}
	if (tmp_global_opts->cakey_str) {
		free(tmp_global_opts->cakey_str);
		tmp_global_opts->cakey_str = NULL;
	}
	if (tmp_global_opts->chain_str) {
		free(tmp_global_opts->chain_str);
		tmp_global_opts->chain_str = NULL;
	}
	if (tmp_global_opts->clientcrt_str) {
		free(tmp_global_opts->clientcrt_str);
		tmp_global_opts->clientcrt_str = NULL;
	}
	if (tmp_global_opts->clientkey_str) {
		free(tmp_global_opts->clientkey_str);
		tmp_global_opts->clientkey_str = NULL;
	}
	if (tmp_global_opts->leafcrlurl_str) {
		free(tmp_global_opts->leafcrlurl_str);
		tmp_global_opts->leafcrlurl_str = NULL;
	}
	if (tmp_global_opts->dh_str) {
		free(tmp_global_opts->dh_str);
		tmp_global_opts->dh_str = NULL;
	}
	free(tmp_global_opts);
}

void
global_free(global_t *global)
{
	if (global->spec) {
		global_proxyspec_free(global->spec);
	}
	if (global->leafcertdir) {
		free(global->leafcertdir);
	}
	if (global->defaultleafcert) {
		cert_free(global->defaultleafcert);
	}
	if (global->dropuser) {
		free(global->dropuser);
	}
	if (global->dropgroup) {
		free(global->dropgroup);
	}
	if (global->jaildir) {
		free(global->jaildir);
	}
	if (global->pidfile) {
		free(global->pidfile);
	}
	if (global->conffile) {
		free(global->conffile);
	}
	if (global->connectlog) {
		free(global->connectlog);
	}
	if (global->contentlog) {
		free(global->contentlog);
	}
	if (global->certgendir) {
		free(global->certgendir);
	}
	if (global->contentlog_basedir) {
		free(global->contentlog_basedir);
	}
	if (global->masterkeylog) {
		free(global->masterkeylog);
	}
	if (global->pcaplog) {
		free(global->pcaplog);
	}
	if (global->pcaplog_basedir) {
		free(global->pcaplog_basedir);
	}
#ifndef WITHOUT_MIRROR
	if (global->mirrorif) {
		free(global->mirrorif);
	}
	if (global->mirrortarget) {
		free(global->mirrortarget);
	}
#endif /* !WITHOUT_MIRROR */
#ifndef WITHOUT_USERAUTH
	if (global->userdb_path) {
		free(global->userdb_path);
	}
	if (global->userdb) {
		// sqlite3.h: "Invoking sqlite3_finalize() on a NULL pointer is a harmless no-op."
		sqlite3_finalize(global->update_user_atime);
		sqlite3_close(global->userdb);
	}
#endif /* !WITHOUT_USERAUTH */
	if (global->opts) {
		opts_free(global->opts);
	}
	if (global->leafkey) {
		EVP_PKEY_free(global->leafkey);
	}
#ifndef OPENSSL_NO_ENGINE
	if (global->openssl_engine) {
		free(global->openssl_engine);
	}
#endif /* !OPENSSL_NO_ENGINE */

	memset(global, 0, sizeof(global_t));
	free(global);
}

/*
 * Return 1 if global_t contains a proxyspec that (eventually) uses SSL/TLS,
 * 0 otherwise.  When 0, it is safe to assume that no SSL/TLS operations
 * will take place with this configuration.
 */
int
global_has_ssl_spec(global_t *global)
{
	proxyspec_t *p = global->spec;
	while (p) {
		if (p->ssl || p->upgrade)
			return 1;
		p = p->next;
	}
	return 0;
}

/*
 * Return 1 if global_t contains a proxyspec with dns, 0 otherwise.
 */
int
global_has_dns_spec(global_t *global)
{
	proxyspec_t *p = global->spec;
	while (p) {
		if (p->dns)
			return 1;
		p = p->next;
	}
	return 0;
}

#ifndef WITHOUT_USERAUTH
/*
 * Return 1 if global_t contains a proxyspec with user_auth, 0 otherwise.
 */
int
global_has_userauth_spec(global_t *global)
{
	proxyspec_t *p = global->spec;
	while (p) {
		if (p->opts->user_auth)
			return 1;
		p = p->next;
	}
	return 0;
}
#endif /* !WITHOUT_USERAUTH */

/*
 * Return 1 if global_t contains a proxyspec with cakey defined, 0 otherwise.
 */
int
global_has_cakey_spec(global_t *global)
{
	proxyspec_t *p = global->spec;
	while (p) {
		if (p->opts->cakey)
			return 1;
		p = p->next;
	}
	return 0;
}

/*
 * Dump the SSL/TLS protocol related configuration.
 */
char *
opts_proto_dbg_dump(opts_t *opts)
{
	char *s;
	if (asprintf(&s, "SSL/TLS protocol: %s%s%s%s%s%s%s%s%s",
#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x20702000L)
#ifdef HAVE_SSLV2
	               (opts->sslmethod == SSLv2_method) ? "ssl2" :
#endif /* HAVE_SSLV2 */
#ifdef HAVE_SSLV3
	               (opts->sslmethod == SSLv3_method) ? "ssl3" :
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	               (opts->sslmethod == TLSv1_method) ? "tls10" :
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	               (opts->sslmethod == TLSv1_1_method) ? "tls11" :
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	               (opts->sslmethod == TLSv1_2_method) ? "tls12" :
#endif /* HAVE_TLSV12 */
/* There is no TLSv1_3_method defined,
 * since no ssl version < 0x10100000L supports it. */
#else /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
#ifdef HAVE_SSLV3
	               (opts->sslversion == SSL3_VERSION) ? "ssl3" :
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	               (opts->sslversion == TLS1_VERSION) ? "tls10" :
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	               (opts->sslversion == TLS1_1_VERSION) ? "tls11" :
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	               (opts->sslversion == TLS1_2_VERSION) ? "tls12" :
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	               (opts->sslversion == TLS1_3_VERSION) ? "tls13" :
#endif /* HAVE_TLSV13 */
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	               "negotiate",
#ifdef HAVE_SSLV2
	               opts->no_ssl2 ? " -ssl2" :
#endif /* HAVE_SSLV2 */
	               "",
#ifdef HAVE_SSLV3
	               opts->no_ssl3 ? " -ssl3" :
#endif /* HAVE_SSLV3 */
	               "",
#ifdef HAVE_TLSV10
	               opts->no_tls10 ? " -tls10" :
#endif /* HAVE_TLSV10 */
	               "",
#ifdef HAVE_TLSV11
	               opts->no_tls11 ? " -tls11" :
#endif /* HAVE_TLSV11 */
	               "",
#ifdef HAVE_TLSV12
	               opts->no_tls12 ? " -tls12" :
#endif /* HAVE_TLSV12 */
	               "",
#ifdef HAVE_TLSV13
	               opts->no_tls13 ? " -tls13" :
#endif /* HAVE_TLSV13 */
	               "",
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER >= 0x20702000L)
#ifdef HAVE_SSLV3
	               (opts->minsslversion == SSL3_VERSION) ? ">=ssl3" :
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	               (opts->minsslversion == TLS1_VERSION) ? ">=tls10" :
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	               (opts->minsslversion == TLS1_1_VERSION) ? ">=tls11" :
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	               (opts->minsslversion == TLS1_2_VERSION) ? ">=tls12" :
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	               (opts->minsslversion == TLS1_3_VERSION) ? ">=tls13" :
#endif /* HAVE_TLSV13 */
	               "",
#ifdef HAVE_SSLV3
	               (opts->maxsslversion == SSL3_VERSION) ? "<=ssl3" :
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	               (opts->maxsslversion == TLS1_VERSION) ? "<=tls10" :
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	               (opts->maxsslversion == TLS1_1_VERSION) ? "<=tls11" :
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	               (opts->maxsslversion == TLS1_2_VERSION) ? "<=tls12" :
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	               (opts->maxsslversion == TLS1_3_VERSION) ? "<=tls13" :
#endif /* HAVE_TLSV13 */
	               ""
#else /* OPENSSL_VERSION_NUMBER < 0x10100000L */
	               "", ""
#endif /* OPENSSL_VERSION_NUMBER < 0x10100000L */
	               ) < 0) {
		s = NULL;
	}
	return s;
}

#ifndef WITHOUT_USERAUTH
static int WUNRES
opts_set_user_auth_url(opts_t *opts, const char * argv0, const char *optarg)
{
	if (opts->user_auth_url)
		free(opts->user_auth_url);
	opts->user_auth_url = strdup(optarg);
	if (!opts->user_auth_url)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("UserAuthURL: %s\n", opts->user_auth_url);
#endif /* DEBUG_OPTS */
	return 0;
}
#endif /* !WITHOUT_USERAUTH */

static opts_t * WUNRES
global_opts_copy(global_t *global, const char *argv0, tmp_global_opts_t *tmp_global_opts)
{
#ifdef DEBUG_OPTS
	log_dbg_printf("Copy global opts\n");
#endif /* DEBUG_OPTS */

	opts_t *opts = opts_new();
	if (!opts)
		return NULL;
	opts->global = global;

	opts->divert = global->opts->divert;
	opts->sslcomp = global->opts->sslcomp;
#ifdef HAVE_SSLV2
	opts->no_ssl2 = global->opts->no_ssl2;
#endif /* HAVE_SSLV2 */
#ifdef HAVE_SSLV3
	opts->no_ssl3 = global->opts->no_ssl3;
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	opts->no_tls10 = global->opts->no_tls10;
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	opts->no_tls11 = global->opts->no_tls11;
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	opts->no_tls12 = global->opts->no_tls12;
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	opts->no_tls13 = global->opts->no_tls13;
#endif /* HAVE_TLSV13 */
	opts->passthrough = global->opts->passthrough;
	opts->deny_ocsp = global->opts->deny_ocsp;
	opts->sslmethod = global->opts->sslmethod;
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER >= 0x20702000L)
	opts->sslversion = global->opts->sslversion;
	opts->minsslversion = global->opts->minsslversion;
	opts->maxsslversion = global->opts->maxsslversion;
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	opts->remove_http_accept_encoding = global->opts->remove_http_accept_encoding;
	opts->remove_http_referer = global->opts->remove_http_referer;
	opts->verify_peer = global->opts->verify_peer;
	opts->allow_wrong_host = global->opts->allow_wrong_host;
#ifndef WITHOUT_USERAUTH
	opts->user_auth = global->opts->user_auth;
	opts->user_timeout = global->opts->user_timeout;
#endif /* !WITHOUT_USERAUTH */
	opts->validate_proto = global->opts->validate_proto;
	opts->max_http_header_size = global->opts->max_http_header_size;

	// Pass NULL as tmp_global_opts param, so we don't reassign the var to itself
	// That would be harmless but incorrect
	if (tmp_global_opts && tmp_global_opts->chain_str) {
		if (opts_set_chain(opts, argv0, tmp_global_opts->chain_str, NULL) == -1)
			return NULL;
	}
	if (tmp_global_opts && tmp_global_opts->leafcrlurl_str) {
		if (opts_set_leafcrlurl(opts, argv0, tmp_global_opts->leafcrlurl_str, NULL) == -1)
			return NULL;
	}
	if (tmp_global_opts && tmp_global_opts->cacrt_str) {
		if (opts_set_cacrt(opts, argv0, tmp_global_opts->cacrt_str, NULL) == -1)
			return NULL;
	}
	if (tmp_global_opts && tmp_global_opts->cakey_str) {
		if (opts_set_cakey(opts, argv0, tmp_global_opts->cakey_str, NULL) == -1)
			return NULL;
	}
	if (tmp_global_opts && tmp_global_opts->clientcrt_str) {
		if (opts_set_clientcrt(opts, argv0, tmp_global_opts->clientcrt_str, NULL) == -1)
			return NULL;
	}
	if (tmp_global_opts && tmp_global_opts->clientkey_str) {
		if (opts_set_clientkey(opts, argv0, tmp_global_opts->clientkey_str, NULL) == -1)
			return NULL;
	}
#ifndef OPENSSL_NO_DH
	if (tmp_global_opts && tmp_global_opts->dh_str) {
		if (opts_set_dh(opts, argv0, tmp_global_opts->dh_str, NULL) == -1)
			return NULL;
	}
#endif /* !OPENSSL_NO_DH */
#ifndef OPENSSL_NO_ECDH
	if (global->opts->ecdhcurve) {
		if (opts_set_ecdhcurve(opts, argv0, global->opts->ecdhcurve) == -1)
			return NULL;
	}
#endif /* !OPENSSL_NO_ECDH */
	if (global->opts->ciphers) {
		if (opts_set_ciphers(opts, argv0, global->opts->ciphers) == -1)
			return NULL;
	}
	if (global->opts->ciphersuites) {
		if (opts_set_ciphersuites(opts, argv0, global->opts->ciphersuites) == -1)
			return NULL;
	}
#ifndef WITHOUT_USERAUTH
	if (global->opts->user_auth_url) {
		if (opts_set_user_auth_url(opts, argv0, global->opts->user_auth_url) == -1)
			return NULL;
	}
	userlist_t *divertusers = global->opts->divertusers;
	while (divertusers) {
		userlist_t *du = malloc(sizeof(userlist_t));
		if (!du)
			return oom_return_null(argv0);
		memset(du, 0, sizeof(userlist_t));

		du->user = strdup(divertusers->user);
		if (!du->user)
			return oom_return_null(argv0);
		du->next = opts->divertusers;
		opts->divertusers = du;

		divertusers = divertusers->next;
	}
	userlist_t *passusers = global->opts->passusers;
	while (passusers) {
		userlist_t *pu = malloc(sizeof(userlist_t));
		if (!pu)
			return oom_return_null(argv0);
		memset(pu, 0, sizeof(userlist_t));

		pu->user = strdup(passusers->user);
		if (!pu->user)
			return oom_return_null(argv0);
		pu->next = opts->passusers;
		opts->passusers = pu;

		passusers = passusers->next;
	}
#endif /* !WITHOUT_USERAUTH */

	if (filter_macro_copy(global->opts->macro, argv0, opts) == -1)
		return oom_return_null(argv0);

	if (filter_rules_copy(global->opts->filter_rules, argv0, opts) == -1)
		return oom_return_null(argv0);

	return opts;
}

proxyspec_t *
proxyspec_new(global_t *global, const char *argv0, tmp_global_opts_t *tmp_global_opts)
{
	proxyspec_t *spec = malloc(sizeof(proxyspec_t));
	if (!spec)
		return oom_return_null(argv0);
	memset(spec, 0, sizeof(proxyspec_t));
	spec->opts = global_opts_copy(global, argv0, tmp_global_opts);
	if (!spec->opts)
		return NULL;
	return spec;
}

int
proxyspec_set_proto(proxyspec_t *spec, const char *value)
{
	/* Defaults */
	spec->ssl = 0;
	spec->http = 0;
	spec->upgrade = 0;
	spec->pop3 = 0;
	spec->smtp = 0;
	if (!strcmp(value, "tcp")) {
		/* use defaults */
	} else
	if (!strcmp(value, "ssl")) {
		spec->ssl = 1;
	} else
	if (!strcmp(value, "http")) {
		spec->http = 1;
	} else
	if (!strcmp(value, "https")) {
		spec->ssl = 1;
		spec->http = 1;
	} else
	if (!strcmp(value, "autossl")) {
		spec->upgrade = 1;
	} else
	if (!strcmp(value, "pop3")) {
		spec->pop3 = 1;
	} else
	if (!strcmp(value, "pop3s")) {
		spec->ssl = 1;
		spec->pop3 = 1;
	} else
	if (!strcmp(value, "smtp")) {
		spec->smtp = 1;
	} else
	if (!strcmp(value, "smtps")) {
		spec->ssl = 1;
		spec->smtp = 1;
	} else {
		fprintf(stderr, "Unknown connection "
						"type '%s'\n", value);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("Proto: %s\n", value);
#endif /* DEBUG_OPTS */
	return 0;
}

static int WUNRES
proxyspec_set_listen_addr(proxyspec_t *spec, char *addr, char *port, const char *natengine)
{
	int af = sys_sockaddr_parse(&spec->listen_addr,
							&spec->listen_addrlen,
							addr, port,
							sys_get_af(addr),
							EVUTIL_AI_PASSIVE);
	if (af == -1) {
		return -1;
	}
	if (natengine) {
		spec->natengine = strdup(natengine);
		if (!spec->natengine)
			return oom_return_na();
	} else {
		spec->natengine = NULL;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("Addr: [%s]:%s, %s\n", addr, port, natengine);
#endif /* DEBUG_OPTS */
	return af;
}

static void
opts_set_divert(opts_t *opts)
{
	opts->divert = 1;
#ifdef DEBUG_OPTS
	log_dbg_printf("Divert: yes\n");
#endif /* DEBUG_OPTS */
}

void
opts_unset_divert(opts_t *opts)
{
	opts->divert = 0;
#ifdef DEBUG_OPTS
	log_dbg_printf("Divert: no\n");
#endif /* DEBUG_OPTS */
}

static int WUNRES
proxyspec_set_divert_addr(proxyspec_t *spec, char *addr, char *port)
{
	if (sys_sockaddr_parse(&spec->conn_dst_addr,
						&spec->conn_dst_addrlen,
						addr, port, AF_INET, EVUTIL_AI_PASSIVE) == -1) {
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("DivertAddr: [%s]:%s\n", addr, port);
#endif /* DEBUG_OPTS */
	return 0;
}
					
static int WUNRES
proxyspec_set_return_addr(proxyspec_t *spec, char *addr)
{
	if (sys_sockaddr_parse(&spec->child_src_addr,
						&spec->child_src_addrlen,
						addr, "0", AF_INET, EVUTIL_AI_PASSIVE) == -1) {
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("ReturnAddr: [%s]\n", addr);
#endif /* DEBUG_OPTS */
	return 0;
}
					
static int WUNRES
proxyspec_set_target_addr(proxyspec_t *spec, char *addr, char *port, int af)
{
	if (sys_sockaddr_parse(&spec->connect_addr,
							&spec->connect_addrlen,
							addr, port, af, 0) == -1) {
		return -1;
	}
	/* explicit target address */
	free(spec->natengine);
	spec->natengine = NULL;
#ifdef DEBUG_OPTS
	log_dbg_printf("TargetAddr: [%s]:%s\n", addr, port);
#endif /* DEBUG_OPTS */
	return 0;
}

static int WUNRES
proxyspec_set_sni_port(proxyspec_t *spec, char *port)
{
	if (!spec->ssl) {
		fprintf(stderr,
				"SNI hostname lookup "
				"only works for ssl "
				"and https proxyspecs"
				"\n");
		return -1;
	}
	/* SNI dstport */
	spec->sni_port = atoi(port);
	if (!spec->sni_port) {
		fprintf(stderr, "Invalid port '%s'\n", port);
		return -1;
	}
	spec->dns = 1;
	free(spec->natengine);
	spec->natengine = NULL;
#ifdef DEBUG_OPTS
	log_dbg_printf("SNIPort: %u\n", spec->sni_port);
#endif /* DEBUG_OPTS */
	return 0;
}

static int WUNRES
proxyspec_set_natengine(proxyspec_t *spec, const char *natengine)
{
	// Double checks if called by proxyspec_parse()
	if (nat_exist(natengine)) {
		/* natengine */
		free(spec->natengine);
		spec->natengine = strdup(natengine);
		if (!spec->natengine) {
			fprintf(stderr, "Out of memory\n");
			return -1;
		}
	} else {
		fprintf(stderr, "No such nat engine '%s'\n", natengine);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("NatEngine: %s\n", spec->natengine);
#endif /* DEBUG_OPTS */
	return 0;
}

static void
set_divert(proxyspec_t *spec, int split)
{
	// The global divert option -n has precedence over the proxyspec Divert option
	// Use split mode if no divert address is specified, even if the Divert option is used
	// The Divert option in structured proxyspecs has precedence over the divert address option (conn_dst_addrlen)
	// If the Divert option is not used in structured proxyspecs, use the global Divert option
	if (split || !spec->conn_dst_addrlen) {
		opts_unset_divert(spec->opts);
	}
}

/*
 * Parse proxyspecs using a simple state machine.
 */
int
proxyspec_parse(int *argc, char **argv[], const char *natengine, global_t *global, const char *argv0, tmp_global_opts_t *tmp_global_opts)
{
	proxyspec_t *spec = NULL;
	char *addr = NULL;
	int state = 0;
	int af;

	while ((*argc)--) {
		switch (state) {
			default:
			case 0:
				/* tcp | ssl | http | https | autossl | pop3 | pop3s | smtp | smtps */
				spec = proxyspec_new(global, argv0, tmp_global_opts);
				if (!spec)
					return -1;
				spec->next = global->spec;
				global->spec = spec;

				if (proxyspec_set_proto(spec, **argv) == -1)
					return -1;
				state++;
				break;
			case 1:
				/* listenaddr */
				addr = **argv;
				state++;
				break;
			case 2:
				/* listenport */
				if ((af = proxyspec_set_listen_addr(spec, addr, **argv, natengine)) == -1)
					return -1;
				state++;
				break;
			case 3:
				state++;
				if (strstr(**argv, "up:")) {
					char *dp = **argv + 3;
					// @todo IPv6?
					char *da = "127.0.0.1";
					char *ra = "127.0.0.1";

					// da and ra are optional, if both specified, da should come before ra
					// Divert address
					if (*argc && strstr(*((*argv) + 1), "ua:")) {
						(*argv)++; (*argc)--;
						da = **argv + 3;
					}
					// Return address
					if (*argc && strstr(*((*argv) + 1), "ra:")) {
						(*argv)++; (*argc)--;
						ra = **argv + 3;
					}

					if (proxyspec_set_divert_addr(spec, da, dp) == -1)
						return -1;
					if (proxyspec_set_return_addr(spec, ra) == -1)
						return -1;
					break;
				}
				/* fall-through */
			case 4:
				/* [ natengine | dstaddr ] */
				if (!strcmp(**argv, "tcp") ||
				    !strcmp(**argv, "ssl") ||
				    !strcmp(**argv, "http") ||
				    !strcmp(**argv, "https") ||
				    !strcmp(**argv, "autossl") ||
				    !strcmp(**argv, "pop3") ||
				    !strcmp(**argv, "pop3s") ||
				    !strcmp(**argv, "smtp") ||
				    !strcmp(**argv, "smtps")) {
					/* implicit default natengine */
					(*argv)--; (*argc)++; /* rewind */
					state = 0;
				} else
				if (!strcmp(**argv, "sni")) {
					state = 6;
				} else
				if (nat_exist(**argv)) {
					/* natengine */
					if (proxyspec_set_natengine(spec, natengine) == -1)
						return -1;
					state = 0;
				} else {
					/* explicit target address */
					addr = **argv;
					state++;
				}
				break;
			case 5:
				/* explicit target port */
				if (proxyspec_set_target_addr(spec, addr, **argv, af) == -1)
					return -1;
				state = 0;
				break;
			case 6:
				/* SNI dstport */
				if (proxyspec_set_sni_port(spec, **argv) == -1)
					return -1;
				state = 0;
				break;
		}
		(*argv)++;
	}

	if (state != 0 && state != 3 && state != 4) {
		fprintf(stderr, "Incomplete proxyspec!\n");
		return -1;
	}

	// Empty line does not create new spec
	if (spec)
		set_divert(spec, tmp_global_opts->split);

	return 0;
}

#ifndef WITHOUT_USERAUTH
static char *
users_str(userlist_t *u)
{
	char *us = NULL;

	if (!u) {
		us = strdup("");
		if (!us)
			return oom_return_na_null();
		goto out;
	}

	while (u) {
		char *nus;
		if (asprintf(&nus, "%s%s%s", STRORNONE(us), us ? "," : "", u->user) < 0) {
			goto err;
		}

		if (us)
			free(us);
		us = nus;
		u = u->next;
	}
	goto out;
err:
	if (us) {
		free(us);
		us = NULL;
	}
out:
	return us;
}
#endif /* !WITHOUT_USERAUTH */

static char *
opts_str(opts_t *opts)
{
	char *s = NULL;
	char *proto_dump = NULL;
	char *ms = NULL;
	char *frs = NULL;
	char *fs = NULL;

#ifndef WITHOUT_USERAUTH
	char *du = NULL;
	char *pu = NULL;

	du = users_str(opts->divertusers);
	if (!du)
		goto out;

	pu = users_str(opts->passusers);
	if (!pu)
		goto out;
#endif /* !WITHOUT_USERAUTH */

	ms = filter_macro_str(opts->macro);
	if (!ms)
		goto out;

	frs = filter_rule_str(opts->filter_rules);
	if (!frs)
		goto out;

	fs = filter_str(opts->filter);
	if (!fs)
		goto out;

	proto_dump = opts_proto_dbg_dump(opts);
	if (!proto_dump)
		goto out;

	if (asprintf(&s, "opts=%s%s"
#ifdef HAVE_SSLV2
				 "%s"
#endif /* HAVE_SSLV2 */
#ifdef HAVE_SSLV3
				 "%s"
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
				 "%s"
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
				 "%s"
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
				 "%s"
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
				 "%s"
#endif /* HAVE_TLSV13 */
				 "%s%s|%s|%s"
#ifndef OPENSSL_NO_ECDH
				 "|%s"
#endif /* !OPENSSL_NO_ECDH */
				 "|%s%s%s%s%s"
#ifndef WITHOUT_USERAUTH
				 "%s|%s|%d|%s|%s"
#endif /* !WITHOUT_USERAUTH */
				 "%s|%d\n%s%s%s%s%s%s%s",
	             (opts->divert ? "divert" : "split"),
	             (!opts->sslcomp ? "|no sslcomp" : ""),
#ifdef HAVE_SSLV2
	             (opts->no_ssl2 ? "|no_ssl2" : ""),
#endif /* HAVE_SSLV2 */
#ifdef HAVE_SSLV3
	             (opts->no_ssl3 ? "|no_ssl3" : ""),
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	             (opts->no_tls10 ? "|no_tls10" : ""),
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	             (opts->no_tls11 ? "|no_tls11" : ""),
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	             (opts->no_tls12 ? "|no_tls12" : ""),
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	             (opts->no_tls13 ? "|no_tls13" : ""),
#endif /* HAVE_TLSV13 */
	             (opts->passthrough ? "|passthrough" : ""),
	             (opts->deny_ocsp ? "|deny_ocsp" : ""),
	             (opts->ciphers ? opts->ciphers : "no ciphers"),
	             (opts->ciphersuites ? opts->ciphersuites : "no ciphersuites"),
#ifndef OPENSSL_NO_ECDH
	             (opts->ecdhcurve ? opts->ecdhcurve : "no ecdhcurve"),
#endif /* !OPENSSL_NO_ECDH */
	             (opts->leafcrlurl ? opts->leafcrlurl : "no leafcrlurl"),
	             (opts->remove_http_accept_encoding ? "|remove_http_accept_encoding" : ""),
	             (opts->remove_http_referer ? "|remove_http_referer" : ""),
	             (opts->verify_peer ? "|verify_peer" : ""),
	             (opts->allow_wrong_host ? "|allow_wrong_host" : ""),
#ifndef WITHOUT_USERAUTH
	             (opts->user_auth ? "|user_auth" : ""),
	             (opts->user_auth_url ? opts->user_auth_url : "no user_auth_url"),
	             opts->user_timeout,
	             du,
	             pu,
#endif /* !WITHOUT_USERAUTH */
	             (opts->validate_proto ? "|validate_proto" : ""),
				 opts->max_http_header_size,
				 proto_dump,
				 strlen(ms) ? "\n" : "", ms,
				 strlen(frs) ? "\n" : "", frs,
				 strlen(fs) ? "\n" : "", fs) < 0) {
		s = NULL;
	}
out:
#ifndef WITHOUT_USERAUTH
	if (du)
		free(du);
	if (pu)
		free(pu);
#endif /* !WITHOUT_USERAUTH */
	if (ms)
		free(ms);
	if (frs)
		free(frs);
	if (fs)
		free(fs);
	if (proto_dump)
		free(proto_dump);
	return s;
}

/*
 * Return text representation of proxy spec for display to the user.
 * Returned string must be freed by caller.
 */
char *
proxyspec_str(proxyspec_t *spec)
{
	char *s = NULL;
	char *lhbuf = NULL;
	char *lpbuf = NULL;
	char *cbuf = NULL;
	char *pdstbuf = NULL;
	char *csrcbuf = NULL;
	char *optsstr = NULL;

	if (sys_sockaddr_str((struct sockaddr *)&spec->listen_addr,
	                     spec->listen_addrlen, &lhbuf, &lpbuf) != 0) {
		goto out;
	}
	if (spec->connect_addrlen) {
		char *chbuf, *cpbuf;
		if (sys_sockaddr_str((struct sockaddr *)&spec->connect_addr,
		                     spec->connect_addrlen,
		                     &chbuf, &cpbuf) != 0) {
			goto out;
		}
		int rv = asprintf(&cbuf, "\nconnect= [%s]:%s", chbuf, cpbuf);
		free(chbuf);
		free(cpbuf);
		if (rv < 0)
			goto out;
	}
	if (spec->conn_dst_addrlen) {
		char *chbuf, *cpbuf;
		if (sys_sockaddr_str((struct sockaddr *)&spec->conn_dst_addr,
		                     spec->conn_dst_addrlen,
		                     &chbuf, &cpbuf) != 0) {
			goto out;
		}
		int rv = asprintf(&pdstbuf, "\nparent dst addr= [%s]:%s", chbuf, cpbuf);
		free(chbuf);
		free(cpbuf);
		if (rv < 0)
			goto out;
	}
	if (spec->child_src_addrlen) {
		char *chbuf, *cpbuf;
		if (sys_sockaddr_str((struct sockaddr *)&spec->child_src_addr,
		                     spec->child_src_addrlen,
		                     &chbuf, &cpbuf) != 0) {
			goto out;
		}
		int rv = asprintf(&csrcbuf, "\nchild src addr= [%s]:%s", chbuf, cpbuf);
		free(chbuf);
		free(cpbuf);
		if (rv < 0)
			goto out;
	}
	if (spec->sni_port) {
		if (asprintf(&cbuf, "\nsni %i", spec->sni_port) < 0) {
			goto out;
		}
	}
	optsstr = opts_str(spec->opts);
	if (!optsstr) {
		goto out;
	}
	if (asprintf(&s, "listen=[%s]:%s %s%s%s%s%s %s%s%s\n%s%s", lhbuf, lpbuf,
	             (spec->ssl ? "ssl" : "tcp"),
	             (spec->upgrade ? "|autossl" : ""),
	             (spec->http ? "|http" : ""),
	             (spec->pop3 ? "|pop3" : ""),
	             (spec->smtp ? "|smtp" : ""),
	             (spec->natengine ? spec->natengine : cbuf),
	             STRORNONE(pdstbuf),
	             STRORNONE(csrcbuf),
	             optsstr,
	             !spec->opts->divert && spec->conn_dst_addrlen ? "\nWARNING: Divert address specified in split mode" : "") < 0) {
		s = NULL;
	}
out:
	if (optsstr)
		free(optsstr);
	if (lhbuf)
		free(lhbuf);
	if (lpbuf)
		free(lpbuf);
	if (cbuf)
		free(cbuf);
	if (pdstbuf)
		free(pdstbuf);
	if (csrcbuf)
		free(csrcbuf);
	return s;
}

int
opts_set_cacrt(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->cacrt_str)
			free(tmp_global_opts->cacrt_str);
		tmp_global_opts->cacrt_str = strdup(optarg);
		if (!tmp_global_opts->cacrt_str)
			return oom_return(argv0);
	}

	if (opts->cacrt)
		X509_free(opts->cacrt);
	opts->cacrt = ssl_x509_load(optarg);
	if (!opts->cacrt) {
		fprintf(stderr, "%s: error loading CA cert from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
	ssl_x509_refcount_inc(opts->cacrt);
	sk_X509_insert(opts->chain, opts->cacrt, 0);
	if (!opts->cakey) {
		opts->cakey = ssl_key_load(optarg);
	}
#ifndef OPENSSL_NO_DH
	if (!opts->dh) {
		opts->dh = ssl_dh_load(optarg);
	}
#endif /* !OPENSSL_NO_DH */
#ifdef DEBUG_OPTS
	log_dbg_printf("CACert: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

int
opts_set_cakey(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->cakey_str)
			free(tmp_global_opts->cakey_str);
		tmp_global_opts->cakey_str = strdup(optarg);
		if (!tmp_global_opts->cakey_str)
			return oom_return(argv0);
	}

	if (opts->cakey)
		EVP_PKEY_free(opts->cakey);
	opts->cakey = ssl_key_load(optarg);
	if (!opts->cakey) {
		fprintf(stderr, "%s: error loading CA key from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
	if (!opts->cacrt) {
		opts->cacrt = ssl_x509_load(optarg);
		if (opts->cacrt) {
			ssl_x509_refcount_inc(opts->cacrt);
			sk_X509_insert(opts->chain, opts->cacrt, 0);
		}
	}
#ifndef OPENSSL_NO_DH
	if (!opts->dh) {
		opts->dh = ssl_dh_load(optarg);
	}
#endif /* !OPENSSL_NO_DH */
#ifdef DEBUG_OPTS
	log_dbg_printf("CAKey: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

int
opts_set_chain(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->chain_str)
			free(tmp_global_opts->chain_str);
		tmp_global_opts->chain_str = strdup(optarg);
		if (!tmp_global_opts->chain_str)
			return oom_return(argv0);
	}

	if (ssl_x509chain_load(NULL, &opts->chain, optarg) == -1) {
		fprintf(stderr, "%s: error loading chain from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("CAChain: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

int
opts_set_leafcrlurl(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->leafcrlurl_str)
			free(tmp_global_opts->leafcrlurl_str);
		tmp_global_opts->leafcrlurl_str = strdup(optarg);
		if (!tmp_global_opts->leafcrlurl_str)
			return oom_return(argv0);
	}

	if (opts->leafcrlurl)
		free(opts->leafcrlurl);
	opts->leafcrlurl = strdup(optarg);
	if (!opts->leafcrlurl)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("LeafCRLURL: %s\n", opts->leafcrlurl);
#endif /* DEBUG_OPTS */
	return 0;
}

static int WUNRES
set_certgendir(global_t *global, const char *argv0, const char *optarg)
{
	if (global->certgendir)
		free(global->certgendir);
	global->certgendir = strdup(optarg);
	if (!global->certgendir)
		return oom_return(argv0);
	return 0;
}

void
opts_set_deny_ocsp(opts_t *opts)
{
	opts->deny_ocsp = 1;
}

void
opts_unset_deny_ocsp(opts_t *opts)
{
	opts->deny_ocsp = 0;
}

void
opts_set_passthrough(opts_t *opts)
{
	opts->passthrough = 1;
}

void
opts_unset_passthrough(opts_t *opts)
{
	opts->passthrough = 0;
}

int
opts_set_clientcrt(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->clientcrt_str)
			free(tmp_global_opts->clientcrt_str);
		tmp_global_opts->clientcrt_str = strdup(optarg);
		if (!tmp_global_opts->clientcrt_str)
			return oom_return(argv0);
	}

	if (opts->clientcrt)
		X509_free(opts->clientcrt);
	opts->clientcrt = ssl_x509_load(optarg);
	if (!opts->clientcrt) {
		fprintf(stderr, "%s: error loading client cert from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("ClientCert: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

int
opts_set_clientkey(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->clientkey_str)
			free(tmp_global_opts->clientkey_str);
		tmp_global_opts->clientkey_str = strdup(optarg);
		if (!tmp_global_opts->clientkey_str)
			return oom_return(argv0);
	}

	if (opts->clientkey)
		EVP_PKEY_free(opts->clientkey);
	opts->clientkey = ssl_key_load(optarg);
	if (!opts->clientkey) {
		fprintf(stderr, "%s: error loading client key from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("ClientKey: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

#ifndef OPENSSL_NO_DH
int
opts_set_dh(opts_t *opts, const char *argv0, const char *optarg, tmp_global_opts_t *tmp_global_opts)
{
	if (tmp_global_opts) {
		if (tmp_global_opts->dh_str)
			free(tmp_global_opts->dh_str);
		tmp_global_opts->dh_str = strdup(optarg);
		if (!tmp_global_opts->dh_str)
			return oom_return(argv0);
	}

	if (opts->dh)
		DH_free(opts->dh);
	opts->dh = ssl_dh_load(optarg);
	if (!opts->dh) {
		fprintf(stderr, "%s: error loading DH params from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("DHGroupParams: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}
#endif /* !OPENSSL_NO_DH */

#ifndef OPENSSL_NO_ECDH
int
opts_set_ecdhcurve(opts_t *opts, const char *argv0, const char *optarg)
{
	EC_KEY *ec;
	if (opts->ecdhcurve)
		free(opts->ecdhcurve);
	if (!(ec = ssl_ec_by_name(optarg))) {
		fprintf(stderr, "%s: unknown curve '%s'\n", argv0, optarg);
		return -1;
	}
	EC_KEY_free(ec);
	opts->ecdhcurve = strdup(optarg);
	if (!opts->ecdhcurve)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("ECDHCurve: %s\n", opts->ecdhcurve);
#endif /* DEBUG_OPTS */
	return 0;
}
#endif /* !OPENSSL_NO_ECDH */

void
opts_set_sslcomp(opts_t *opts)
{
	opts->sslcomp = 1;
}

void
opts_unset_sslcomp(opts_t *opts)
{
	opts->sslcomp = 0;
}

int
opts_set_ciphers(opts_t *opts, const char *argv0, const char *optarg)
{
	if (opts->ciphers)
		free(opts->ciphers);
	opts->ciphers = strdup(optarg);
	if (!opts->ciphers)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("Ciphers: %s\n", opts->ciphers);
#endif /* DEBUG_OPTS */
	return 0;
}

int
opts_set_ciphersuites(opts_t *opts, const char *argv0, const char *optarg)
{
	if (opts->ciphersuites)
		free(opts->ciphersuites);
	opts->ciphersuites = strdup(optarg);
	if (!opts->ciphersuites)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("CipherSuites: %s\n", opts->ciphersuites);
#endif /* DEBUG_OPTS */
	return 0;
}

/*
 * Parse SSL proto string in optarg and look up the corresponding SSL method.
 */
int
opts_force_proto(opts_t *opts, const char *argv0, const char *optarg)
{
#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x20702000L)
	if (opts->sslmethod != SSLv23_method) {
#else /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	if (opts->sslversion) {
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
		fprintf(stderr, "%s: cannot use -r multiple times\n", argv0);
		return -1;
	}

#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x20702000L)
#ifdef HAVE_SSLV2
	if (!strcmp(optarg, "ssl2")) {
		opts->sslmethod = SSLv2_method;
	} else
#endif /* HAVE_SSLV2 */
#ifdef HAVE_SSLV3
	if (!strcmp(optarg, "ssl3")) {
		opts->sslmethod = SSLv3_method;
	} else
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	if (!strcmp(optarg, "tls10") || !strcmp(optarg, "tls1")) {
		opts->sslmethod = TLSv1_method;
	} else
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	if (!strcmp(optarg, "tls11")) {
		opts->sslmethod = TLSv1_1_method;
	} else
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	if (!strcmp(optarg, "tls12")) {
		opts->sslmethod = TLSv1_2_method;
	} else
#endif /* HAVE_TLSV12 */
/* There is no TLSv1_3_method defined,
 * since no ssl version < 0x10100000L supports it. */
#else /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
/*
 * Support for SSLv2 and the corresponding SSLv2_method(),
 * SSLv2_server_method() and SSLv2_client_method() functions were
 * removed in OpenSSL 1.1.0.
 */
#ifdef HAVE_SSLV3
	if (!strcmp(optarg, "ssl3")) {
		opts->sslversion = SSL3_VERSION;
	} else
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	if (!strcmp(optarg, "tls10") || !strcmp(optarg, "tls1")) {
		opts->sslversion = TLS1_VERSION;
	} else
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	if (!strcmp(optarg, "tls11")) {
		opts->sslversion = TLS1_1_VERSION;
	} else
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	if (!strcmp(optarg, "tls12")) {
		opts->sslversion = TLS1_2_VERSION;
	} else
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	if (!strcmp(optarg, "tls13")) {
		opts->sslversion = TLS1_3_VERSION;
	} else
#endif /* HAVE_TLSV13 */
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	{
		fprintf(stderr, "%s: Unsupported SSL/TLS protocol '%s'\n",
		                argv0, optarg);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("ForceSSLProto: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

/*
 * Parse SSL proto string in optarg and set the corresponding no_foo bit.
 */
int
opts_disable_proto(opts_t *opts, const char *argv0, const char *optarg)
{
#ifdef HAVE_SSLV2
	if (!strcmp(optarg, "ssl2")) {
		opts->no_ssl2 = 1;
	} else
#endif /* HAVE_SSLV2 */
#ifdef HAVE_SSLV3
	if (!strcmp(optarg, "ssl3")) {
		opts->no_ssl3 = 1;
	} else
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	if (!strcmp(optarg, "tls10") || !strcmp(optarg, "tls1")) {
		opts->no_tls10 = 1;
	} else
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	if (!strcmp(optarg, "tls11")) {
		opts->no_tls11 = 1;
	} else
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	if (!strcmp(optarg, "tls12")) {
		opts->no_tls12 = 1;
	} else
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	if (!strcmp(optarg, "tls13")) {
		opts->no_tls13 = 1;
	} else
#endif /* HAVE_TLSV13 */
	{
		fprintf(stderr, "%s: Unsupported SSL/TLS protocol '%s'\n",
		                argv0, optarg);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("DisableSSLProto: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

static int WUNRES
opts_set_min_proto(UNUSED opts_t *opts, const char *argv0, const char *optarg)
{
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER >= 0x20702000L)
#ifdef HAVE_SSLV3
	if (!strcmp(optarg, "ssl3")) {
		opts->minsslversion = SSL3_VERSION;
	} else
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	if (!strcmp(optarg, "tls10") || !strcmp(optarg, "tls1")) {
		opts->minsslversion = TLS1_VERSION;
	} else
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	if (!strcmp(optarg, "tls11")) {
		opts->minsslversion = TLS1_1_VERSION;
	} else
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	if (!strcmp(optarg, "tls12")) {
		opts->minsslversion = TLS1_2_VERSION;
	} else
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	if (!strcmp(optarg, "tls13")) {
		opts->minsslversion = TLS1_3_VERSION;
	} else
#endif /* HAVE_TLSV13 */
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	{
		fprintf(stderr, "%s: Unsupported SSL/TLS protocol '%s'\n",
		                argv0, optarg);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("MinSSLProto: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

static int WUNRES
opts_set_max_proto(UNUSED opts_t *opts, const char *argv0, const char *optarg)
{
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)) || (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER >= 0x20702000L)
#ifdef HAVE_SSLV3
	if (!strcmp(optarg, "ssl3")) {
		opts->maxsslversion = SSL3_VERSION;
	} else
#endif /* HAVE_SSLV3 */
#ifdef HAVE_TLSV10
	if (!strcmp(optarg, "tls10") || !strcmp(optarg, "tls1")) {
		opts->maxsslversion = TLS1_VERSION;
	} else
#endif /* HAVE_TLSV10 */
#ifdef HAVE_TLSV11
	if (!strcmp(optarg, "tls11")) {
		opts->maxsslversion = TLS1_1_VERSION;
	} else
#endif /* HAVE_TLSV11 */
#ifdef HAVE_TLSV12
	if (!strcmp(optarg, "tls12")) {
		opts->maxsslversion = TLS1_2_VERSION;
	} else
#endif /* HAVE_TLSV12 */
#ifdef HAVE_TLSV13
	if (!strcmp(optarg, "tls13")) {
		opts->maxsslversion = TLS1_3_VERSION;
	} else
#endif /* HAVE_TLSV13 */
#endif /* OPENSSL_VERSION_NUMBER >= 0x10100000L */
	{
		fprintf(stderr, "%s: Unsupported SSL/TLS protocol '%s'\n",
		                argv0, optarg);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("MaxSSLProto: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

static void
opts_set_remove_http_accept_encoding(opts_t *opts)
{
	opts->remove_http_accept_encoding = 1;
}

static void
opts_unset_remove_http_accept_encoding(opts_t *opts)
{
	opts->remove_http_accept_encoding = 0;
}

static void
opts_set_remove_http_referer(opts_t *opts)
{
	opts->remove_http_referer = 1;
}

static void
opts_unset_remove_http_referer(opts_t *opts)
{
	opts->remove_http_referer = 0;
}

static void
opts_set_verify_peer(opts_t *opts)
{
	opts->verify_peer = 1;
}

static void
opts_unset_verify_peer(opts_t *opts)
{
	opts->verify_peer = 0;
}

static void
opts_set_allow_wrong_host(opts_t *opts)
{
	opts->allow_wrong_host = 1;
}

static void
opts_unset_allow_wrong_host(opts_t *opts)
{
	opts->allow_wrong_host = 0;
}

#ifndef WITHOUT_USERAUTH
static void
opts_set_user_auth(UNUSED opts_t *opts)
{
#if defined(__OpenBSD__) || defined(__linux__)
	// Enable user auth on OpenBSD and Linux only
	opts->user_auth = 1;
#endif /* __OpenBSD__ || __linux__ */
}

static void
opts_unset_user_auth(opts_t *opts)
{
	opts->user_auth = 0;
}
#endif /* !WITHOUT_USERAUTH */

static void
opts_set_validate_proto(opts_t *opts)
{
	opts->validate_proto = 1;
}

static void
opts_unset_validate_proto(opts_t *opts)
{
	opts->validate_proto = 0;
}

#ifndef WITHOUT_USERAUTH
// Limit the number of users to max 50
#define MAX_USERS 50

static int WUNRES
opts_set_userlist(char *value, int line_num, userlist_t **list, const char *listname)
{
	// Delimiter can be either or all of ",", " ", and "\t"
	// Using space as a delimiter disables spaces in user names too
	// user1[,user2[,user3]]
	char *argv[sizeof(char *) * MAX_USERS];
	int argc = 0;
	char *p, *last = NULL;

	// strtok_r() removes all delimiters around user names, and does not return empty tokens
	for ((p = strtok_r(value, ", \t", &last));
		 p;
		 (p = strtok_r(NULL, ", \t", &last))) {
		if (argc < MAX_USERS) {
			argv[argc++] = p;
		} else {
			fprintf(stderr, "Too many arguments in user list, max users allowed %d, on line %d\n", MAX_USERS, line_num);
			return -1;
		}
	}

	if (!argc) {
		fprintf(stderr, "%s requires at least one parameter on line %d\n", listname, line_num);
		return -1;
	}

	// Override the copied global list, if any
	if (*list) {
		free_userlist(*list);
		*list = NULL;
	}

	while (argc--) {
		userlist_t *ul = malloc(sizeof(userlist_t));
		if (!ul)
			return oom_return_na();
		memset(ul, 0, sizeof(userlist_t));

		ul->user = strdup(argv[argc]);
		if (!ul->user)
			return oom_return_na();
		ul->next = *list;
		*list = ul;
	}
	return 0;
}
#endif /* !WITHOUT_USERAUTH */

int
global_set_leafkey(global_t *global, const char *argv0, const char *optarg)
{
	if (global->leafkey)
		EVP_PKEY_free(global->leafkey);
	global->leafkey = ssl_key_load(optarg);
	if (!global->leafkey) {
		fprintf(stderr, "%s: error loading leaf key from '%s':\n",
		        argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
#ifndef OPENSSL_NO_DH
	if (!global->opts->dh) {
		global->opts->dh = ssl_dh_load(optarg);
	}
#endif /* !OPENSSL_NO_DH */
#ifdef DEBUG_OPTS
	log_dbg_printf("LeafKey: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

#ifndef OPENSSL_NO_ENGINE
int
global_set_openssl_engine(global_t *global, const char *argv0, const char *optarg)
{
	if (global->openssl_engine)
		free(global->openssl_engine);
	global->openssl_engine = strdup(optarg);
	if (!global->openssl_engine)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("OpenSSLEngine: %s\n", global->openssl_engine);
#endif /* DEBUG_OPTS */
	return 0;
}
#endif /* !OPENSSL_NO_ENGINE */

int
global_set_leafcertdir(global_t *global, const char *argv0, const char *optarg)
{
	if (!sys_isdir(optarg)) {
		fprintf(stderr, "%s: '%s' is not a directory\n",
		        argv0, optarg);
		return -1;
	}
	if (global->leafcertdir)
		free(global->leafcertdir);
	global->leafcertdir = strdup(optarg);
	if (!global->leafcertdir)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("LeafCertDir: %s\n", global->leafcertdir);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_defaultleafcert(global_t *global, const char *argv0, const char *optarg)
{
	if (global->defaultleafcert)
		cert_free(global->defaultleafcert);
	global->defaultleafcert = opts_load_cert_chain_key(optarg);
	if (!global->defaultleafcert) {
		fprintf(stderr, "%s: error loading default leaf cert/chain/key"
		                " from '%s':\n", argv0, optarg);
		if (errno) {
			fprintf(stderr, "%s\n", strerror(errno));
		} else {
			ERR_print_errors_fp(stderr);
		}
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("DefaultLeafCert: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_certgendir_writegencerts(global_t *global, const char *argv0,
                                  const char *optarg)
{
	global->certgen_writeall = 0;
	if (set_certgendir(global, argv0, optarg) == -1)
		return -1;
#ifdef DEBUG_OPTS
	log_dbg_printf("WriteGenCertsDir: certgendir=%s, writeall=%u\n",
	               global->certgendir, global->certgen_writeall);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_certgendir_writeall(global_t *global, const char *argv0,
                             const char *optarg)
{
	global->certgen_writeall = 1;
	if (set_certgendir(global, argv0, optarg) == -1)
		return -1;
#ifdef DEBUG_OPTS
	log_dbg_printf("WriteAllCertsDir: certgendir=%s, writeall=%u\n",
	               global->certgendir, global->certgen_writeall);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_user(global_t *global, const char *argv0, const char *optarg)
{
	if (!sys_isuser(optarg)) {
		fprintf(stderr, "%s: '%s' is not an existing user\n",
		        argv0, optarg);
		return -1;
	}
	if (global->dropuser)
		free(global->dropuser);
	global->dropuser = strdup(optarg);
	if (!global->dropuser)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("User: %s\n", global->dropuser);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_group(global_t *global, const char *argv0, const char *optarg)
{
	if (!sys_isgroup(optarg)) {
		fprintf(stderr, "%s: '%s' is not an existing group\n",
		        argv0, optarg);
		return -1;
	}
	if (global->dropgroup)
		free(global->dropgroup);
	global->dropgroup = strdup(optarg);
	if (!global->dropgroup)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("Group: %s\n", global->dropgroup);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_jaildir(global_t *global, const char *argv0, const char *optarg)
{
	if (!sys_isdir(optarg)) {
		fprintf(stderr, "%s: '%s' is not a directory\n", argv0, optarg);
		return -1;
	}
	if (global->jaildir)
		free(global->jaildir);
	global->jaildir = realpath(optarg, NULL);
	if (!global->jaildir) {
		fprintf(stderr, "%s: Failed to realpath '%s': %s (%i)\n",
		        argv0, optarg, strerror(errno), errno);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("Chroot: %s\n", global->jaildir);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_pidfile(global_t *global, const char *argv0, const char *optarg)
{
	if (global->pidfile)
		free(global->pidfile);
	global->pidfile = strdup(optarg);
	if (!global->pidfile)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("PidFile: %s\n", global->pidfile);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_connectlog(global_t *global, const char *argv0, const char *optarg)
{
	if (global->connectlog)
		free(global->connectlog);
	if (!(global->connectlog = sys_realdir(optarg))) {
		if (errno == ENOENT) {
			fprintf(stderr, "Directory part of '%s' does not "
			                "exist\n", optarg);
			return -1;
		} else {
			fprintf(stderr, "Failed to realpath '%s': %s (%i)\n",
			              optarg, strerror(errno), errno);
			return oom_return(argv0);
		}
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("ConnectLog: %s\n", global->connectlog);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_contentlog(global_t *global, const char *argv0, const char *optarg)
{
	if (global->contentlog)
		free(global->contentlog);
	if (!(global->contentlog = sys_realdir(optarg))) {
		if (errno == ENOENT) {
			fprintf(stderr, "Directory part of '%s' does not "
			                "exist\n", optarg);
			return -1;
		} else {
			fprintf(stderr, "Failed to realpath '%s': %s (%i)\n",
			              optarg, strerror(errno), errno);
			return oom_return(argv0);
		}
	}
	global->contentlog_isdir = 0;
	global->contentlog_isspec = 0;
#ifdef DEBUG_OPTS
	log_dbg_printf("ContentLog: %s\n", global->contentlog);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_contentlogdir(global_t *global, const char *argv0, const char *optarg)
{
	if (!sys_isdir(optarg)) {
		fprintf(stderr, "%s: '%s' is not a directory\n", argv0, optarg);
		return -1;
	}
	if (global->contentlog)
		free(global->contentlog);
	global->contentlog = realpath(optarg, NULL);
	if (!global->contentlog) {
		fprintf(stderr, "%s: Failed to realpath '%s': %s (%i)\n",
		        argv0, optarg, strerror(errno), errno);
		return -1;
	}
	global->contentlog_isdir = 1;
	global->contentlog_isspec = 0;
#ifdef DEBUG_OPTS
	log_dbg_printf("ContentLogDir: %s\n", global->contentlog);
#endif /* DEBUG_OPTS */
	return 0;
}

static int
global_set_logbasedir(const char *argv0, const char *optarg,
                    char **basedir, char **log)
{
	char *lhs, *rhs, *p, *q;
	size_t n;
	if (*basedir)
		free(*basedir);
	if (*log)
		free(*log);
	if (log_content_split_pathspec(optarg, &lhs, &rhs) == -1) {
		fprintf(stderr, "%s: Failed to split '%s' in lhs/rhs:"
		                " %s (%i)\n", argv0, optarg,
		                strerror(errno), errno);
		return -1;
	}
	/* eliminate %% from lhs */
	for (p = q = lhs; *p; p++, q++) {
		if (q < p)
			*q = *p;
		if (*p == '%' && *(p+1) == '%')
			p++;
	}
	*q = '\0';
	/* all %% in lhs resolved to % */
	if (sys_mkpath(lhs, 0777) == -1) {
		fprintf(stderr, "%s: Failed to create '%s': %s (%i)\n",
		        argv0, lhs, strerror(errno), errno);
		return -1;
	}
	*basedir = realpath(lhs, NULL);
	if (!*basedir) {
		fprintf(stderr, "%s: Failed to realpath '%s': %s (%i)\n",
		        argv0, lhs, strerror(errno), errno);
		return -1;
	}
	/* count '%' in basedir */
	for (n = 0, p = *basedir;
		 *p;
		 p++) {
		if (*p == '%')
			n++;
	}
	free(lhs);
	n += strlen(*basedir);
	if (!(lhs = malloc(n + 1)))
		return oom_return(argv0);
	/* re-encoding % to %%, copying basedir to lhs */
	for (p = *basedir, q = lhs;
		 *p;
		 p++, q++) {
		*q = *p;
		if (*q == '%')
			*(++q) = '%';
	}
	*q = '\0';
	/* lhs contains encoded realpathed basedir */
	if (asprintf(log, "%s/%s", lhs, rhs) < 0)
		return oom_return(argv0);
	free(lhs);
	free(rhs);
	return 0;
}

int
global_set_contentlogpathspec(global_t *global, const char *argv0, const char *optarg)
{
	if (global_set_logbasedir(argv0, optarg, &global->contentlog_basedir, &global->contentlog) == -1)
		return -1;
	global->contentlog_isdir = 0;
	global->contentlog_isspec = 1;
#ifdef DEBUG_OPTS
	log_dbg_printf("ContentLogPathSpec: basedir=%s, %s\n",
	               global->contentlog_basedir, global->contentlog);
#endif /* DEBUG_OPTS */
	return 0;
}

#ifdef HAVE_LOCAL_PROCINFO
void
global_set_lprocinfo(global_t *global)
{
	global->lprocinfo = 1;
}

void
global_unset_lprocinfo(global_t *global)
{
	global->lprocinfo = 0;
}
#endif /* HAVE_LOCAL_PROCINFO */

int
global_set_masterkeylog(global_t *global, const char *argv0, const char *optarg)
{
	if (global->masterkeylog)
		free(global->masterkeylog);
	if (!(global->masterkeylog = sys_realdir(optarg))) {
		if (errno == ENOENT) {
			fprintf(stderr, "Directory part of '%s' does not "
			                "exist\n", optarg);
			return -1;
		} else {
			fprintf(stderr, "Failed to realpath '%s': %s (%i)\n",
			              optarg, strerror(errno), errno);
			return oom_return(argv0);
		}
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("MasterKeyLog: %s\n", global->masterkeylog);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_pcaplog(global_t *global, const char *argv0, const char *optarg)
{
	if (global->pcaplog)
		free(global->pcaplog);
	if (!(global->pcaplog = sys_realdir(optarg))) {
		if (errno == ENOENT) {
			fprintf(stderr, "Directory part of '%s' does not "
			                "exist\n", optarg);
			return -1;
		} else {
			fprintf(stderr, "Failed to realpath '%s': %s (%i)\n",
			              optarg, strerror(errno), errno);
			return oom_return(argv0);
		}
	}
	global->pcaplog_isdir = 0;
	global->pcaplog_isspec = 0;
#ifdef DEBUG_OPTS
	log_dbg_printf("PcapLog: %s\n", global->pcaplog);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_pcaplogdir(global_t *global, const char *argv0, const char *optarg)
{
	if (!sys_isdir(optarg)) {
		fprintf(stderr, "%s: '%s' is not a directory\n", argv0, optarg);
		return -1;
	}
	if (global->pcaplog)
		free(global->pcaplog);
	global->pcaplog = realpath(optarg, NULL);
	if (!global->pcaplog) {
		fprintf(stderr, "%s: Failed to realpath '%s': %s (%i)\n",
		        argv0, optarg, strerror(errno), errno);
		return -1;
	}
	global->pcaplog_isdir = 1;
	global->pcaplog_isspec = 0;
#ifdef DEBUG_OPTS
	log_dbg_printf("PcapLogDir: %s\n", global->pcaplog);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_pcaplogpathspec(global_t *global, const char *argv0, const char *optarg)
{
	if (global_set_logbasedir(argv0, optarg, &global->pcaplog_basedir, &global->pcaplog) == -1)
		return -1;
	global->pcaplog_isdir = 0;
	global->pcaplog_isspec = 1;
#ifdef DEBUG_OPTS
	log_dbg_printf("PcapLogPathSpec: basedir=%s, %s\n",
	               global->pcaplog_basedir, global->pcaplog);
#endif /* DEBUG_OPTS */
	return 0;
}

#ifndef WITHOUT_MIRROR
int
global_set_mirrorif(global_t *global, const char *argv0, const char *optarg)
{
	if (global->mirrorif)
		free(global->mirrorif);
	global->mirrorif = strdup(optarg);
	if (!global->mirrorif)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("MirrorIf: %s\n", global->mirrorif);
#endif /* DEBUG_OPTS */
	return 0;
}

int
global_set_mirrortarget(global_t *global, const char *argv0, const char *optarg)
{
	if (global->mirrortarget)
		free(global->mirrortarget);
	global->mirrortarget = strdup(optarg);
	if (!global->mirrortarget)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("MirrorTarget: %s\n", global->mirrortarget);
#endif /* DEBUG_OPTS */
	return 0;
}
#endif /* !WITHOUT_MIRROR */

void
global_set_daemon(global_t *global)
{
	global->detach = 1;
}

void
global_unset_daemon(global_t *global)
{
	global->detach = 0;
}

void
global_set_debug(global_t *global)
{
	log_dbg_mode(LOG_DBG_MODE_ERRLOG);
	global->debug = 1;
}

void
global_unset_debug(global_t *global)
{
	log_dbg_mode(LOG_DBG_MODE_NONE);
	global->debug = 0;
}

int
global_set_debug_level(const char *optarg)
{
	if (equal(optarg, "2")) {
		log_dbg_mode(LOG_DBG_MODE_FINE);
	} else if (equal(optarg, "3")) {
		log_dbg_mode(LOG_DBG_MODE_FINER);
	} else if (equal(optarg, "4")) {
		log_dbg_mode(LOG_DBG_MODE_FINEST);
	} else {
		fprintf(stderr, "Invalid DebugLevel '%s', use 2-4\n", optarg);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("DebugLevel: %s\n", optarg);
#endif /* DEBUG_OPTS */
	return 0;
}

void
global_set_statslog(global_t *global)
{
	global->statslog = 1;
}

void
global_unset_statslog(global_t *global)
{
	global->statslog = 0;
}

#ifndef WITHOUT_USERAUTH
static int WUNRES
global_set_userdb_path(global_t *global, const char *argv0, const char *optarg)
{
	if (global->userdb_path)
		free(global->userdb_path);
	global->userdb_path = strdup(optarg);
	if (!global->userdb_path)
		return oom_return(argv0);
#ifdef DEBUG_OPTS
	log_dbg_printf("UserDBPath: %s\n", global->userdb_path);
#endif /* DEBUG_OPTS */
	return 0;
}
#endif /* !WITHOUT_USERAUTH */

int
is_yesno(const char *value)
{
	if (equal(value, "yes"))
		return 1;
	else if (equal(value, "no"))
		return 0;
	return -1;
}

static int
check_value_yesno(const char *value, const char *name, int line_num)
{
	int rv;
	if ((rv = is_yesno(value)) == -1)
		fprintf(stderr, "Error in conf: Invalid '%s' value '%s' on line %d, use yes|no\n", name, value, line_num);
	return rv;
}

/*
 * global_opt param is used to save certain global opts, so that we can use 
 * them cloning global opts while creating proxyspecs
 */
static int
set_option(opts_t *opts, const char *argv0,
		const char *name, char *value, char **natengine, int line_num, tmp_global_opts_t *tmp_global_opts)
{
	int yes;

	if (!value || !strlen(value)) {
		fprintf(stderr, "Error in conf: No value assigned for %s on line %d\n", name, line_num);
		return -1;
	}

	if (equal(name, "CACert")) {
		return opts_set_cacrt(opts, argv0, value, tmp_global_opts);
	} else if (equal(name, "CAKey")) {
		return opts_set_cakey(opts, argv0, value, tmp_global_opts);
	} else if (equal(name, "ClientCert")) {
		return opts_set_clientcrt(opts, argv0, value, tmp_global_opts);
	} else if (equal(name, "ClientKey")) {
		return opts_set_clientkey(opts, argv0, value, tmp_global_opts);
	} else if (equal(name, "CAChain")) {
		return opts_set_chain(opts, argv0, value, tmp_global_opts);
	} else if (equal(name, "LeafCRLURL")) {
		return opts_set_leafcrlurl(opts, argv0, value, tmp_global_opts);
	} else if (equal(name, "DenyOCSP")) {
		yes = check_value_yesno(value, "DenyOCSP", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_deny_ocsp(opts) : opts_unset_deny_ocsp(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("DenyOCSP: %u\n", opts->deny_ocsp);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "Passthrough")) {
		yes = check_value_yesno(value, "Passthrough", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_passthrough(opts) : opts_unset_passthrough(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("Passthrough: %u\n", opts->passthrough);
#endif /* DEBUG_OPTS */
#ifndef OPENSSL_NO_DH
	} else if (equal(name, "DHGroupParams")) {
		return opts_set_dh(opts, argv0, value, tmp_global_opts);
#endif /* !OPENSSL_NO_DH */
#ifndef OPENSSL_NO_ECDH
	} else if (equal(name, "ECDHCurve")) {
		return opts_set_ecdhcurve(opts, argv0, value);
#endif /* !OPENSSL_NO_ECDH */
#ifdef SSL_OP_NO_COMPRESSION
	} else if (equal(name, "SSLCompression")) {
		yes = check_value_yesno(value, "SSLCompression", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_sslcomp(opts) : opts_unset_sslcomp(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("SSLCompression: %u\n", opts->sslcomp);
#endif /* DEBUG_OPTS */
#endif /* SSL_OP_NO_COMPRESSION */
	} else if (equal(name, "ForceSSLProto")) {
		return opts_force_proto(opts, argv0, value);
	} else if (equal(name, "DisableSSLProto")) {
		return opts_disable_proto(opts, argv0, value);
	} else if (equal(name, "MinSSLProto")) {
		return opts_set_min_proto(opts, argv0, value);
	} else if (equal(name, "MaxSSLProto")) {
		return opts_set_max_proto(opts, argv0, value);
	} else if (equal(name, "Ciphers")) {
		return opts_set_ciphers(opts, argv0, value);
	} else if (equal(name, "CipherSuites")) {
		return opts_set_ciphersuites(opts, argv0, value);
	} else if (equal(name, "NATEngine")) {
		if (*natengine)
			free(*natengine);
		*natengine = strdup(value);
		if (!*natengine)
			return oom_return(argv0);
#ifdef DEBUG_OPTS
		log_dbg_printf("NATEngine: %s\n", *natengine);
#endif /* DEBUG_OPTS */
#ifndef WITHOUT_USERAUTH
	} else if (equal(name, "UserAuth")) {
		yes = check_value_yesno(value, "UserAuth", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_user_auth(opts) : opts_unset_user_auth(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("UserAuth: %u\n", opts->user_auth);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "UserAuthURL")) {
		return opts_set_user_auth_url(opts, argv0, value);
	} else if (equal(name, "UserTimeout")) {
		unsigned int i = atoi(value);
		if (i <= 86400) {
			opts->user_timeout = i;
		} else {
			fprintf(stderr, "Invalid UserTimeout %s on line %d, use 0-86400\n", value, line_num);
			return -1;
		}
#ifdef DEBUG_OPTS
		log_dbg_printf("UserTimeout: %u\n", opts->user_timeout);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "DivertUsers")) {
		return opts_set_userlist(value, line_num, &opts->divertusers, "DivertUsers");
	} else if (equal(name, "PassUsers")) {
		return opts_set_userlist(value, line_num, &opts->passusers, "PassUsers");
#endif /* !WITHOUT_USERAUTH */
	} else if (equal(name, "ValidateProto")) {
		yes = check_value_yesno(value, "ValidateProto", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_validate_proto(opts) : opts_unset_validate_proto(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("ValidateProto: %u\n", opts->validate_proto);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "MaxHTTPHeaderSize")) {
		unsigned int i = atoi(value);
		if (i >= 1024 && i <= 65536) {
			opts->max_http_header_size = i;
		} else {
			fprintf(stderr, "Invalid MaxHTTPHeaderSize %s on line %d, use 1024-65536\n", value, line_num);
			return -1;
		}
#ifdef DEBUG_OPTS
		log_dbg_printf("MaxHTTPHeaderSize: %u\n", opts->max_http_header_size);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "VerifyPeer")) {
		yes = check_value_yesno(value, "VerifyPeer", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_verify_peer(opts) : opts_unset_verify_peer(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("VerifyPeer: %u\n", opts->verify_peer);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "AllowWrongHost")) {
		yes = check_value_yesno(value, "AllowWrongHost", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_allow_wrong_host(opts) : opts_unset_allow_wrong_host(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("AllowWrongHost: %u\n", opts->allow_wrong_host);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "RemoveHTTPAcceptEncoding")) {
		yes = check_value_yesno(value, "RemoveHTTPAcceptEncoding", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_remove_http_accept_encoding(opts) : opts_unset_remove_http_accept_encoding(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("RemoveHTTPAcceptEncoding: %u\n", opts->remove_http_accept_encoding);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "RemoveHTTPReferer")) {
		yes = check_value_yesno(value, "RemoveHTTPReferer", line_num);
		if (yes == -1)
			return -1;
		yes ? opts_set_remove_http_referer(opts) : opts_unset_remove_http_referer(opts);
#ifdef DEBUG_OPTS
		log_dbg_printf("RemoveHTTPReferer: %u\n", opts->remove_http_referer);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "PassSite")) {
		return filter_passsite_set(opts, value, line_num);
	} else if (equal(name, "Define")) {
		return filter_macro_set(opts, value, line_num);
	} else if (equal(name, "Split") || equal(name, "Pass") || equal(name, "Block") || equal(name, "Match")) {
		return filter_rule_set(opts, name, value, line_num);
	} else if (equal(name, "Divert")) {
		yes = is_yesno(value);
		if (yes == -1)
			return filter_rule_set(opts, name, value, line_num);
		else
			yes ? opts_set_divert(opts) : opts_unset_divert(opts);
	} else {
		fprintf(stderr, "Error in conf: Unknown option "
		                "'%s' on line %d\n", name, line_num);
		return -1;
	}
	return 0;
}

static int WUNRES
set_proxyspec_option(proxyspec_t *spec, const char *argv0,
		const char *name, char *value, char **natengine, spec_addrs_t *spec_addrs, int line_num)
{
	// Closing brace '}' is the only option without a value
	// and only allowed in structured proxyspecs
	if ((!value || !strlen(value)) && !equal(name, "}")) {
		fprintf(stderr, "Error in conf: No value assigned for %s on line %d\n", name, line_num);
		return -1;
	}

	if (equal(name, "Proto")) {
		if (proxyspec_set_proto(spec, value) == -1)
			return -1;
	}
	else if (equal(name, "Addr")) {
		spec_addrs->addr = strdup(value);
		if (!spec_addrs->addr)
			return oom_return(argv0);
	}
	else if (equal(name, "Port")) {
		if (spec_addrs->addr) {
			spec_addrs->af = proxyspec_set_listen_addr(spec, spec_addrs->addr, value, *natengine);
		} else {
			fprintf(stderr, "ProxySpec Port without Addr on line %d\n", line_num);
			return -1;
		}
	}
	else if (equal(name, "DivertAddr")) {
		spec_addrs->divert_addr = strdup(value);
		if (!spec_addrs->divert_addr)
			return oom_return(argv0);
	}
	else if (equal(name, "DivertPort")) {
		if (spec_addrs->divert_addr) {
			if (proxyspec_set_divert_addr(spec, spec_addrs->divert_addr, value) == -1)
				return -1;
		} else {
			if (proxyspec_set_divert_addr(spec, "127.0.0.1", value) == -1)
				return -1;
		}
	}
	else if (equal(name, "ReturnAddr")) {
		if (proxyspec_set_return_addr(spec, value) == -1)
			return -1;
	}
	else if (equal(name, "TargetAddr")) {
		spec_addrs->target_addr = strdup(value);
		if (!spec_addrs->target_addr)
			return oom_return(argv0);
	}
	else if (equal(name, "TargetPort")) {
		if (spec_addrs->target_addr) {
			if (proxyspec_set_target_addr(spec, spec_addrs->target_addr, value, spec_addrs->af) == -1)
				return -1;
		} else {
			fprintf(stderr, "ProxySpec TargetPort without TargetAddr on line %d\n", line_num);
			return -1;
		}
	}
	else if (equal(name, "SNIPort")) {
		if (proxyspec_set_sni_port(spec, value) == -1)
			return -1;
	}
	else if (equal(name, "NatEngine")) {
		if (proxyspec_set_natengine(spec, value) == -1)
			return -1;
	}
	else if (equal(name, "}")) {
#ifdef DEBUG_OPTS
		log_dbg_printf("ProxySpec } on line %d\n", line_num);
#endif /* DEBUG_OPTS */
		if (!spec_addrs->addr || !spec_addrs->af) {
			fprintf(stderr, "Incomplete ProxySpec on line %d\n", line_num);
			return -1;
		}
		// Return 2 to indicate the end of structured proxyspec
		return 2;
	}
	else {
		return set_option(spec->opts, argv0, name, value, natengine, line_num, NULL);
	}
	return 0;
}

/*
 * Separator param is needed for command line options only.
 * Conf file option separator is ' ', on the command line is '='.
 * Allows multiple separators between name and value.
 */
int
get_name_value(char *name, char **value, const char sep, int line_num)
{
	size_t len = strlen(name);

	// Find end of name and null-terminate
	char *n = name;
	while (*n != '\0' && *n != ' ' && *n != '\t' && *n != '\r' && *n != '\n' && *n != sep)
		n++;
	*n = '\0';

	size_t name_len = strlen(name);

	if (!name_len) {
		fprintf(stderr, "Error in option: No option name on line %d\n", line_num);
		// Return empty value
		*value = name;
		return -1;
	}

	if (len == name_len) {
#ifdef DEBUG_OPTS
		log_dbg_printf("Warning in option: No option separator on line %d\n", line_num);
#endif /* DEBUG_OPTS */
		// Return empty value
		*value = name + name_len;
		return 0;
	}

	// Trim left of value (skip white space and sep until value)
	do n++;
	while (*n == ' ' || *n == '\t' || *n == '\r' || *n == '\n' || *n == sep);

	*value = n;

	size_t value_len = strlen(*value);

	if (!value_len) {
#ifdef DEBUG_OPTS
		log_dbg_printf("Warning in option: No option value on line %d\n", line_num);
#endif /* DEBUG_OPTS */
		return 0;
	}

	// Trim right of value
	n = *value + value_len - 1;
	while (*n == ' ' || *n == '\t' || *n == '\r' || *n == '\n' || *n == sep)
		n--;
	*(n + 1) = '\0';

	return 0;
}

#define MAX_TOKENS 8

static int WUNRES
load_proxyspec_line(global_t *global, const char *argv0, char *value, char **natengine, int line_num, tmp_global_opts_t *tmp_global_opts)
{
	/* Use MAX_TOKENS instead of computing the actual number of tokens in value */
	char **argv = malloc(sizeof(char *) * MAX_TOKENS);
	if (!argv)
		return oom_return(argv0);
	char **save_argv = argv;
	int argc = 0;
	char *p, *last = NULL;

	for ((p = strtok_r(value, " ", &last));
		 p;
		 (p = strtok_r(NULL, " ", &last))) {
		/* Limit max # token */
		if (argc < MAX_TOKENS) {
			argv[argc++] = p;
		} else {
			fprintf(stderr, "Too many arguments in proxyspec on line %d\n", line_num);
			return -1;
		}
	}

	if (proxyspec_parse(&argc, &argv, *natengine, global, argv0, tmp_global_opts) == -1) {
		fprintf(stderr, "Error in proxyspec on line %d\n", line_num);
		return -1;
	}

	free(save_argv);
	return 0;
}

static int WUNRES
load_proxyspec_struct(global_t *global, const char *argv0, char **natengine, int *line_num, FILE *f, tmp_global_opts_t *tmp_global_opts)
{
	int retval = -1;
	char *name, *value;
	char *line = NULL;
	size_t line_len;

	proxyspec_t *spec = proxyspec_new(global, argv0, tmp_global_opts);
	if (!spec)
		return -1;
	spec->next = global->spec;
	global->spec = spec;

	// Set the default return addr
	if (proxyspec_set_return_addr(spec, "127.0.0.1") == -1)
		return  -1;

	spec_addrs_t *spec_addrs = malloc(sizeof(spec_addrs_t));
	if (!spec_addrs)
		return oom_return(argv0);
	memset(spec_addrs, 0, sizeof(spec_addrs_t));

	int closing_brace = 0;

	while (!feof(f) && !closing_brace) {
		if (getline(&line, &line_len, f) == -1) {
			break;
		}
		if (line == NULL) {
			fprintf(stderr, "Error in conf file: getline() returns NULL line after line %d\n", *line_num);
			goto leave;
		}
		(*line_num)++;

		/* Skip white space */
		for (name = line; *name == ' ' || *name == '\t'; name++); 

		/* Skip comments and empty lines */
		if ((name[0] == '\0') || (name[0] == '#') || (name[0] == ';') ||
			(name[0] == '\r') || (name[0] == '\n')) {
			continue;
		}

		retval = get_name_value(name, &value, ' ', *line_num);
		if (retval == 0) {
			retval = set_proxyspec_option(spec, argv0, name, value, natengine, spec_addrs, *line_num);
		}
		if (retval == -1) {
			goto leave;
		} else if (retval == 2) {
			closing_brace = 1;
		}
		free(line);
		line = NULL;
	}

	if (!closing_brace) {
		fprintf(stderr, "Error in conf file: struct ProxySpec has no closing brace '}' after line %d\n", *line_num);
		retval = -1;
		goto leave;
	}

	set_divert(spec, tmp_global_opts->split);

	retval = 0;
leave:
	if (line)
		free(line);
	spec_addrs_free(spec_addrs);
	return retval;
}

static int WUNRES
global_set_open_files_limit(const char *value, int line_num)
{
	unsigned int i = atoi(value);
	if (i >= 50 && i <= 10000) {
		struct rlimit rl;
		rl.rlim_cur = i;
		rl.rlim_max = i;
		if (setrlimit(RLIMIT_NOFILE, &rl) == -1) {
			fprintf(stderr, "Failed setting OpenFilesLimit\n");
			if (errno) {
				fprintf(stderr, "%s\n", strerror(errno));
			} else {
				ERR_print_errors_fp(stderr);
			}
			return -1;
		}
	} else {
		fprintf(stderr, "Invalid OpenFilesLimit %s on line %d, use 50-10000\n", value, line_num);
		return -1;
	}
#ifdef DEBUG_OPTS
	log_dbg_printf("OpenFilesLimit: %u\n", i);
#endif /* DEBUG_OPTS */
	return 0;
}


static int
opts_load_conffile(global_t *global, const char *argv0, char *conffile, char **natengine, tmp_global_opts_t *tmp_global_opts);

static int WUNRES
set_global_option(global_t *global, const char *argv0,
           const char *name, char *value, char **natengine, int *line_num, FILE *f, tmp_global_opts_t *tmp_global_opts)
{
	int yes;

	if (!value || !strlen(value)) {
		fprintf(stderr, "Error in conf: No value assigned for %s on line %d\n", name, *line_num);
		return -1;
	}

	if (equal(name, "LeafCertDir")) {
		return global_set_leafcertdir(global, argv0, value);
	} else if (equal(name, "DefaultLeafCert")) {
		return global_set_defaultleafcert(global, argv0, value);
	} else if (equal(name, "WriteGenCertsDir")) {
		return global_set_certgendir_writegencerts(global, argv0, value);
	} else if (equal(name, "WriteAllCertsDir")) {
		return global_set_certgendir_writeall(global, argv0, value);
	} else if (equal(name, "User")) {
		return global_set_user(global, argv0, value);
	} else if (equal(name, "Group")) {
		return global_set_group(global, argv0, value);
	} else if (equal(name, "Chroot")) {
		return global_set_jaildir(global, argv0, value);
	} else if (equal(name, "PidFile")) {
		return global_set_pidfile(global, argv0, value);
	} else if (equal(name, "ConnectLog")) {
		return global_set_connectlog(global, argv0, value);
	} else if (equal(name, "ContentLog")) {
		return global_set_contentlog(global, argv0, value);
	} else if (equal(name, "ContentLogDir")) {
		return global_set_contentlogdir(global, argv0, value);
	} else if (equal(name, "ContentLogPathSpec")) {
		return global_set_contentlogpathspec(global, argv0, value);
#ifdef HAVE_LOCAL_PROCINFO
	} else if (equal(name, "LogProcInfo")) {
		yes = check_value_yesno(value, "LogProcInfo", *line_num);
		if (yes == -1)
			return -1;
		yes ? global_set_lprocinfo(global) : global_unset_lprocinfo(global);
#ifdef DEBUG_OPTS
		log_dbg_printf("LogProcInfo: %u\n", global->lprocinfo);
#endif /* DEBUG_OPTS */
#endif /* HAVE_LOCAL_PROCINFO */
	} else if (equal(name, "MasterKeyLog")) {
		return global_set_masterkeylog(global, argv0, value);
	} else if (equal(name, "PcapLog")) {
		return global_set_pcaplog(global, argv0, value);
	} else if (equal(name, "PcapLogDir")) {
		return global_set_pcaplogdir(global, argv0, value);
	} else if (equal(name, "PcapLogPathSpec")) {
		return global_set_pcaplogpathspec(global, argv0, value);
#ifndef WITHOUT_MIRROR
	} else if (equal(name, "MirrorIf")) {
		return global_set_mirrorif(global, argv0, value);
	} else if (equal(name, "MirrorTarget")) {
		return global_set_mirrortarget(global, argv0, value);
#endif /* !WITHOUT_MIRROR */
	} else if (equal(name, "Daemon")) {
		yes = check_value_yesno(value, "Daemon", *line_num);
		if (yes == -1)
			return -1;
		yes ? global_set_daemon(global) : global_unset_daemon(global);
#ifdef DEBUG_OPTS
		log_dbg_printf("Daemon: %u\n", global->detach);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "Debug")) {
		yes = check_value_yesno(value, "Debug", *line_num);
		if (yes == -1)
			return -1;
		yes ? global_set_debug(global) : global_unset_debug(global);
#ifdef DEBUG_OPTS
		log_dbg_printf("Debug: %u\n", global->debug);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "DebugLevel")) {
		return global_set_debug_level(value);
#ifndef WITHOUT_USERAUTH
	} else if (equal(name, "UserDBPath")) {
		return global_set_userdb_path(global, argv0, value);
#endif /* !WITHOUT_USERAUTH */
	} else if (equal(name, "ProxySpec")) {
		if (equal(value, "{")) {
#ifdef DEBUG_OPTS
			log_dbg_printf("ProxySpec { on line %d\n", *line_num);
#endif /* DEBUG_OPTS */
			return load_proxyspec_struct(global, argv0, natengine, line_num, f, tmp_global_opts);
		} else {
			return load_proxyspec_line(global, argv0, value, natengine, *line_num, tmp_global_opts);
		}
	} else if (equal(name, "ConnIdleTimeout")) {
		unsigned int i = atoi(value);
		if (i >= 10 && i <= 3600) {
			global->conn_idle_timeout = i;
		} else {
			fprintf(stderr, "Invalid ConnIdleTimeout %s on line %d, use 10-3600\n", value, *line_num);
			return -1;
		}
#ifdef DEBUG_OPTS
		log_dbg_printf("ConnIdleTimeout: %u\n", global->conn_idle_timeout);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "ExpiredConnCheckPeriod")) {
		unsigned int i = atoi(value);
		if (i >= 10 && i <= 60) {
			global->expired_conn_check_period = i;
		} else {
			fprintf(stderr, "Invalid ExpiredConnCheckPeriod %s on line %d, use 10-60\n", value, *line_num);
			return -1;
		}
#ifdef DEBUG_OPTS
		log_dbg_printf("ExpiredConnCheckPeriod: %u\n", global->expired_conn_check_period);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "LogStats")) {
		yes = check_value_yesno(value, "LogStats", *line_num);
		if (yes == -1)
			return -1;
		yes ? global_set_statslog(global) : global_unset_statslog(global);
#ifdef DEBUG_OPTS
		log_dbg_printf("LogStats: %u\n", global->statslog);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "StatsPeriod")) {
		unsigned int i = atoi(value);
		if (i >= 1 && i <= 10) {
			global->stats_period = i;
		} else {
			fprintf(stderr, "Invalid StatsPeriod %s on line %d, use 1-10\n", value, *line_num);
			return -1;
		}
#ifdef DEBUG_OPTS
		log_dbg_printf("StatsPeriod: %u\n", global->stats_period);
#endif /* DEBUG_OPTS */
	} else if (equal(name, "OpenFilesLimit")) {
		return global_set_open_files_limit(value, *line_num);
	} else if (equal(name, "LeafKey")) {
		return global_set_leafkey(global, argv0, value);
	} else if (equal(name, "LeafKeyRSABits")) {
		unsigned int i = atoi(value);
		if (i == 1024 || i == 2048 || i == 3072 || i == 4096) {
			global->leafkey_rsabits = i;
		} else {
			fprintf(stderr, "Invalid LeafKeyRSABits %s on line %d, use 1024|2048|3072|4096\n", value, *line_num);
			return -1;
		}
#ifdef DEBUG_OPTS
		log_dbg_printf("LeafKeyRSABits: %u\n", global->leafkey_rsabits);
#endif /* DEBUG_OPTS */
#ifndef OPENSSL_NO_ENGINE
	} else if (equal(name, "OpenSSLEngine")) {
		return global_set_openssl_engine(global, argv0, value);
#endif /* !OPENSSL_NO_ENGINE */
	} else if (equal(name, "Include")) {
		// Prevent infinitely recursive include files
		if (tmp_global_opts->include) {
			fprintf(stderr, "Include option not allowed in include files '%s' on line %d\n", value, *line_num);
			return -1;
		}

		tmp_global_opts->include = 1;
		int retval = opts_load_conffile(global, argv0, value, natengine, tmp_global_opts);
		tmp_global_opts->include = 0;

		if (retval == -1) {
			fprintf(stderr, "Error in include file '%s' on line %d\n", value, *line_num);
		}
		return retval;
	} else {
		return set_option(global->opts, argv0, name, value, natengine, *line_num, tmp_global_opts);
	}
	return 0;
}

int
global_set_option(global_t *global, const char *argv0, const char *optarg,
		char **natengine, tmp_global_opts_t *tmp_global_opts)
{
	char *name, *value;
	int retval = -1;
	char *line = strdup(optarg);
	if (!line)
		return oom_return(argv0);

	/* White spaces possible before option name,
	 * if the command line option is passed between the quotes */
	for (name = line; *name == ' ' || *name == '\t'; name++); 

	/* Command line option separator is '=' */
	retval = get_name_value(name, &value, '=', 0);
	if (retval == 0) {
		/* Line number param is for conf file, pass 0 for command line options */
		int line_num = 0;
		retval = set_global_option(global, argv0, name, value, natengine, &line_num, NULL, tmp_global_opts);
	}

	if (line)
		free(line);
	return retval;
}

static int WUNRES
opts_load_conffile(global_t *global, const char *argv0, char *conffile, char **natengine, tmp_global_opts_t *tmp_global_opts)
{
	int retval, line_num;
	char *line, *name, *value;
	size_t line_len;
	FILE *f;
	
#ifdef DEBUG_OPTS
	log_dbg_printf("Conf file: %s\n", conffile);
#endif /* DEBUG_OPTS */

	f = fopen(conffile, "r");
	if (!f) {
		fprintf(stderr, "Error opening conf file '%s': %s\n", conffile, strerror(errno));
		return -1;
	}

	line = NULL;
	line_num = 0;
	retval = -1;
	while (!feof(f)) {
		if (getline(&line, &line_len, f) == -1) {
			break;
		}
		if (line == NULL) {
			fprintf(stderr, "Error in conf file: getline() returns NULL line after line %d\n", line_num);
			goto leave;
		}
		line_num++;

		/* Skip white space */
		for (name = line; *name == ' ' || *name == '\t'; name++); 

		/* Skip comments and empty lines */
		if ((name[0] == '\0') || (name[0] == '#') || (name[0] == ';') ||
			(name[0] == '\r') || (name[0] == '\n')) {
			continue;
		}

		retval = get_name_value(name, &value, ' ', line_num);
		if (retval == 0) {
			retval = set_global_option(global, argv0, name, value, natengine, &line_num, f, tmp_global_opts);
		}

		if (retval == -1) {
			goto leave;
		}
		free(line);
		line = NULL;
	}

leave:
	fclose(f);
	if (line)
		free(line);
	return retval;
}

int
global_load_conffile(global_t *global, const char *argv0, const char *optarg, char **natengine, tmp_global_opts_t *tmp_global_opts)
{
	if (global->conffile)
		free(global->conffile);
	global->conffile = strdup(optarg);
	if (!global->conffile)
		return oom_return(argv0);
	int retval = opts_load_conffile(global, argv0, global->conffile, natengine, tmp_global_opts);
	if (retval == -1)
		fprintf(stderr, "Error in conf file '%s'\n", global->conffile);
	return retval;
}

/* vim: set noet ft=c: */
