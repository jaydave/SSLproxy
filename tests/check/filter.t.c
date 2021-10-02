/*-
 * SSLproxy
 *
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

#include "attrib.h"
#include "opts.h"
#include "filter.h"

#include <check.h>
#include <unistd.h>

START_TEST(set_filter_rule_01)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("*");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_02)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("from ip *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from ip 192.168.0.1");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip 192.168.0.1");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip 192.168.0.1");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip 192.168.0.1");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from ip 192.168.0.1");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("$macro 192.168.0.1 192.168.0.2");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	// macro expansion returns 1, not 0
	s = strdup("from ip $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from ip $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from ip $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from ip $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from ip $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	
	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_03)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("$macro root daemon");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	close(2);

	s = strdup("from user *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);

	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);

	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);

	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);

	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == -1, "failed to parse rule");
	free(s);

	opts->user_auth = 1;

	s = strdup("from user *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("from user * desc keyword");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc keyword");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("from user $macro desc $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	
	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_04)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("to ip *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to ip * port *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to ip 192.168.0.1");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to ip 192.168.0.1 port *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to ip * port 443");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port 443");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port 443");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port 443");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip * port 443");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to ip 192.168.0.1 port 443");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port 443");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port 443");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port 443");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to ip 192.168.0.1 port 443");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("$macro1 192.168.0.1 192.168.0.2");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("$macro2 443 444");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("to ip $macro1 port $macro2");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to ip $macro1 port $macro2");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to ip $macro1 port $macro2");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to ip $macro1 port $macro2");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to ip $macro1 port $macro2");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_05)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("$macro example.com example*");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("to sni *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to sni example.com");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni example.com");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni example.com");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni example.com");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to sni example.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to sni $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to sni $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to sni $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to sni $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to sni $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("to cn *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to cn example.com");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn example.com");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn example.com");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn example.com");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to cn example.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to cn $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to cn $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to cn $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to cn $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to cn $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("to host *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to host example.com");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host example.com");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host example.com");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host example.com");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to host example.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to host $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to host $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to host $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to host $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to host $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("to uri *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to uri example.com");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri example.com");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri example.com");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri example.com");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("to uri example.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("to uri $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to uri $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to uri $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to uri $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("to uri $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_06)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("log *");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log connect");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log connect");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log connect");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log connect");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log connect");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log master");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log master");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log master");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log master");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log master");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log cert");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log cert");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log cert");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log cert");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log cert");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log content");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log content");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log content");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log content");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log content");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log pcap");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log pcap");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log pcap");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log pcap");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log pcap");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log mirror");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log mirror");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log mirror");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log mirror");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log mirror");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !*");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !*");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !*");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !*");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !*");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !connect");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !connect");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !connect");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !connect");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !connect");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !master");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !master");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !master");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !master");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !master");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !cert");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !cert");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !cert");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !cert");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !cert");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !content");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !content");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !content");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !content");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !content");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !pcap");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !pcap");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !pcap");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !pcap");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !pcap");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("log !mirror");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !mirror");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !mirror");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !mirror");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);
	s = strdup("log !mirror");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("$macro connect master cert content pcap mirror");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("log $macro");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("$macro2 !connect !master !cert !content !pcap !mirror");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("log $macro2");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro2");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro2");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro2");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro2");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("$macro3 connect !master cert !content pcap !mirror");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("log $macro3");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro3");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro3");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro3");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro3");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("$macro4 !connect master !cert content !pcap mirror");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("log $macro4");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro4");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro4");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro4");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro4");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("$macro5 connect master cert !content !pcap !mirror");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("log $macro5");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro5");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro5");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro5");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro5");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = strdup("$macro6 !connect !master !cert content pcap mirror");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("log $macro6");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro6");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro6");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro6");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);
	s = strdup("log $macro6");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_07)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("*");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from *");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from ip *");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	opts->user_auth = 1;

	s = strdup("from user *");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user * desc desc");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from * to * log *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=, port=, ip=, user=, keyword=, exact=||||, all=conns||sites|, action=divert||||, log=|||||, apply to=dstip|sni|cn|host|uri, precedence=0\n"
		"filter rule 1: site=, port=, ip=, user=, keyword=, exact=||||, all=conns||sites|, action=|split|||, log=|||||, apply to=dstip|sni|cn|host|uri, precedence=0\n"
		"filter rule 2: site=, port=, ip=, user=, keyword=, exact=||||, all=conns||sites|, action=||pass||, log=|||||, apply to=dstip|sni|cn|host|uri, precedence=0\n"
		"filter rule 3: site=, port=, ip=, user=, keyword=, exact=||||, all=|users|sites|, action=|||block|, log=|||||, apply to=dstip|sni|cn|host|uri, precedence=1\n"
		"filter rule 4: site=, port=, ip=, user=, keyword=desc, exact=||||keyword, all=|users|sites|, action=||||match, log=|||||, apply to=dstip|sni|cn|host|uri, precedence=2\n"
		"filter rule 5: site=, port=, ip=, user=, keyword=, exact=||||, all=conns||sites|, action=||||match, log=connect|master|cert|content|pcap|mirror, apply to=dstip|sni|cn|host|uri, precedence=1"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);
	
	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"   keyword 0 desc (exact)= \n"
"    ip substring: \n"
"      0:  (all_sites, substring, action=||||match, log=|||||, precedence=2)\n"
"    sni substring: \n"
"      0:  (all_sites, substring, action=||||match, log=|||||, precedence=2)\n"
"    cn substring: \n"
"      0:  (all_sites, substring, action=||||match, log=|||||, precedence=2)\n"
"    host substring: \n"
"      0:  (all_sites, substring, action=||||match, log=|||||, precedence=2)\n"
"    uri substring: \n"
"      0:  (all_sites, substring, action=||||match, log=|||||, precedence=2)\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"    ip substring: \n"
"      0:  (all_sites, substring, action=|||block|, log=|||||, precedence=1)\n"
"    sni substring: \n"
"      0:  (all_sites, substring, action=|||block|, log=|||||, precedence=1)\n"
"    cn substring: \n"
"      0:  (all_sites, substring, action=|||block|, log=|||||, precedence=1)\n"
"    host substring: \n"
"      0:  (all_sites, substring, action=|||block|, log=|||||, precedence=1)\n"
"    uri substring: \n"
"      0:  (all_sites, substring, action=|||block|, log=|||||, precedence=1)\n"
"ip_filter_exact->\n"
"ip_filter_substr->\n"
"all_filter->\n"
"    ip substring: \n"
"      0:  (all_sites, substring, action=divert|split|pass||match, log=connect|master|cert|content|pcap|mirror, precedence=1)\n"
"    sni substring: \n"
"      0:  (all_sites, substring, action=divert|split|pass||match, log=connect|master|cert|content|pcap|mirror, precedence=1)\n"
"    cn substring: \n"
"      0:  (all_sites, substring, action=divert|split|pass||match, log=connect|master|cert|content|pcap|mirror, precedence=1)\n"
"    host substring: \n"
"      0:  (all_sites, substring, action=divert|split|pass||match, log=connect|master|cert|content|pcap|mirror, precedence=1)\n"
"    uri substring: \n"
"      0:  (all_sites, substring, action=divert|split|pass||match, log=connect|master|cert|content|pcap|mirror, precedence=1)\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_08)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2 log connect master cert content pcap mirror");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2 log !connect !cert !pcap");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Block action at precedence 1 is not applied to a site of the same rule at precedence 2 now
	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target
	s = strdup("from ip 192.168.0.1 to ip 192.168.0.3");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another source
	s = strdup("from ip 192.168.0.2 to ip 192.168.0.1");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// all_sites should always be the last element
	s = strdup("from ip 192.168.0.2 to ip *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Search substring (subnet?)
	s = strdup("from ip 192.168.0.2 to ip 192.168.0.*");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target
	s = strdup("from ip 192.168.0.2 to ip 192.168.0.3");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=192.168.0.2, port=, ip=192.168.0.1, user=, keyword=, exact=site||ip||, all=|||, action=divert||||, log=|||||, apply to=dstip||||, precedence=1\n"
		"filter rule 1: site=192.168.0.2, port=, ip=192.168.0.1, user=, keyword=, exact=site||ip||, all=|||, action=|split|||, log=connect|master|cert|content|pcap|mirror, apply to=dstip||||, precedence=2\n"
		"filter rule 2: site=192.168.0.2, port=, ip=192.168.0.1, user=, keyword=, exact=site||ip||, all=|||, action=||pass||, log=!connect||!cert||!pcap|, apply to=dstip||||, precedence=2\n"
		"filter rule 3: site=192.168.0.2, port=, ip=192.168.0.1, user=, keyword=, exact=site||ip||, all=|||, action=|||block|, log=|||||, apply to=dstip||||, precedence=1\n"
		"filter rule 4: site=192.168.0.3, port=, ip=192.168.0.1, user=, keyword=, exact=site||ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=1\n"
		"filter rule 5: site=192.168.0.1, port=, ip=192.168.0.2, user=, keyword=, exact=site||ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=1\n"
		"filter rule 6: site=, port=, ip=192.168.0.2, user=, keyword=, exact=||ip||, all=||sites|, action=||||match, log=|||||, apply to=dstip||||, precedence=1\n"
		"filter rule 7: site=192.168.0., port=, ip=192.168.0.2, user=, keyword=, exact=||ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=1\n"
		"filter rule 8: site=192.168.0.3, port=, ip=192.168.0.2, user=, keyword=, exact=site||ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=1"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);

	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"ip_filter_exact->\n"
"  ip 0 192.168.0.2 (exact)= \n"
"    ip exact: \n"
"      0: 192.168.0.3 (exact, action=||||match, log=|||||, precedence=1)\n"
"      1: 192.168.0.1 (exact, action=||||match, log=|||||, precedence=1)\n"
"    ip substring: \n"
"      0: 192.168.0. (substring, action=||||match, log=|||||, precedence=1)\n"
"      1:  (all_sites, substring, action=||||match, log=|||||, precedence=1)\n"
"  ip 1 192.168.0.1 (exact)= \n"
"    ip exact: \n"
"      0: 192.168.0.3 (exact, action=||||match, log=|||||, precedence=1)\n"
"      1: 192.168.0.2 (exact, action=divert|split|pass||, log=!connect|master|!cert|content|!pcap|mirror, precedence=2)\n"
"ip_filter_substr->\n"
"all_filter->\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_09)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2 port 443");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2 port 443 log connect master cert content pcap mirror");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2 port 443 log !connect !cert !pcap");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Block action at precedence 2 is not applied to a port of the same rule at precedence 3 now
	s = strdup("from ip 192.168.0.1 to ip 192.168.0.2 port 443");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target, the following port rules should not change this site rule
	s = strdup("from ip 192.168.0.1 to ip 192.168.0.3 log !mirror");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target port
	s = strdup("from ip 192.168.0.1 to ip 192.168.0.3 port 443");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target port
	s = strdup("from ip 192.168.0.1 to ip 192.168.0.3 port 80");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another source
	s = strdup("from ip 192.168.0.2 to ip 192.168.0.1 port 443");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// all_sites should always be the last element
	s = strdup("from ip 192.168.0.2 to ip 192.168.0.1 port *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Search substring
	s = strdup("from ip 192.168.0.2 to ip 192.168.0.1 port 80*");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=192.168.0.2, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=divert||||, log=|||||, apply to=dstip||||, precedence=2\n"
		"filter rule 1: site=192.168.0.2, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=|split|||, log=connect|master|cert|content|pcap|mirror, apply to=dstip||||, precedence=3\n"
		"filter rule 2: site=192.168.0.2, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||pass||, log=!connect||!cert||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 3: site=192.168.0.2, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=|||block|, log=|||||, apply to=dstip||||, precedence=2\n"
		"filter rule 4: site=192.168.0.3, port=, ip=192.168.0.1, user=, keyword=, exact=site||ip||, all=|||, action=||||match, log=|||||!mirror, apply to=dstip||||, precedence=2\n"
		"filter rule 5: site=192.168.0.3, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=2\n"
		"filter rule 6: site=192.168.0.3, port=80, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=2\n"
		"filter rule 7: site=192.168.0.1, port=443, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=2\n"
		"filter rule 8: site=192.168.0.1, port=, ip=192.168.0.2, user=, keyword=, exact=site||ip||, all=|||ports, action=||||match, log=|||||, apply to=dstip||||, precedence=2\n"
		"filter rule 9: site=192.168.0.1, port=80, ip=192.168.0.2, user=, keyword=, exact=site||ip||, all=|||, action=||||match, log=|||||, apply to=dstip||||, precedence=2"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);

	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"ip_filter_exact->\n"
"  ip 0 192.168.0.2 (exact)= \n"
"    ip exact: \n"
"      0: 192.168.0.1 (exact, action=||||, log=|||||, precedence=0)\n"
"        port exact:\n"
"          0: 443 (exact, action=||||match, log=|||||, precedence=2)\n"
"        port substring:\n"
"          0: 80 (substring, action=||||match, log=|||||, precedence=2)\n"
"          1:  (all_ports, substring, action=||||match, log=|||||, precedence=2)\n"
"  ip 1 192.168.0.1 (exact)= \n"
"    ip exact: \n"
"      0: 192.168.0.3 (exact, action=||||match, log=|||||!mirror, precedence=2)\n"
"        port exact:\n"
"          0: 80 (exact, action=||||match, log=|||||, precedence=2)\n"
"          1: 443 (exact, action=||||match, log=|||||, precedence=2)\n"
"      1: 192.168.0.2 (exact, action=||||, log=|||||, precedence=0)\n"
"        port exact:\n"
"          0: 443 (exact, action=divert|split|pass||, log=!connect|master|!cert|content|!pcap|mirror, precedence=3)\n"
"ip_filter_substr->\n"
"all_filter->\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_10)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	opts->user_auth = 1;

	s = strdup("from user root to sni example.com");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user root to sni example.com log connect master cert content pcap mirror");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user root to sni example.com log !connect !cert !pcap");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Block action at precedence 2 is not applied to a site of the same rule at precedence 4 now
	s = strdup("from user root to sni example.com");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target
	s = strdup("from user root to sni example2.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another source
	s = strdup("from user daemon to sni example.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// all_sites should always be the last element
	s = strdup("from user daemon to sni *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Search substring (subdomain?)
	s = strdup("from user daemon to sni .example.com*");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target
	s = strdup("from user daemon to sni example3.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=example.com, port=, ip=, user=root, keyword=, exact=site|||user|, all=|||, action=divert||||, log=|||||, apply to=|sni|||, precedence=3\n"
		"filter rule 1: site=example.com, port=, ip=, user=root, keyword=, exact=site|||user|, all=|||, action=|split|||, log=connect|master|cert|content|pcap|mirror, apply to=|sni|||, precedence=4\n"
		"filter rule 2: site=example.com, port=, ip=, user=root, keyword=, exact=site|||user|, all=|||, action=||pass||, log=!connect||!cert||!pcap|, apply to=|sni|||, precedence=4\n"
		"filter rule 3: site=example.com, port=, ip=, user=root, keyword=, exact=site|||user|, all=|||, action=|||block|, log=|||||, apply to=|sni|||, precedence=3\n"
		"filter rule 4: site=example2.com, port=, ip=, user=root, keyword=, exact=site|||user|, all=|||, action=||||match, log=|||||, apply to=|sni|||, precedence=3\n"
		"filter rule 5: site=example.com, port=, ip=, user=daemon, keyword=, exact=site|||user|, all=|||, action=||||match, log=|||||, apply to=|sni|||, precedence=3\n"
		"filter rule 6: site=, port=, ip=, user=daemon, keyword=, exact=|||user|, all=||sites|, action=||||match, log=|||||, apply to=|sni|||, precedence=3\n"
		"filter rule 7: site=.example.com, port=, ip=, user=daemon, keyword=, exact=|||user|, all=|||, action=||||match, log=|||||, apply to=|sni|||, precedence=3\n"
		"filter rule 8: site=example3.com, port=, ip=, user=daemon, keyword=, exact=site|||user|, all=|||, action=||||match, log=|||||, apply to=|sni|||, precedence=3"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);

	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"  user 0 root (exact)= \n"
"    sni exact: \n"
"      0: example2.com (exact, action=||||match, log=|||||, precedence=3)\n"
"      1: example.com (exact, action=divert|split|pass||, log=!connect|master|!cert|content|!pcap|mirror, precedence=4)\n"
"  user 1 daemon (exact)= \n"
"    sni exact: \n"
"      0: example3.com (exact, action=||||match, log=|||||, precedence=3)\n"
"      1: example.com (exact, action=||||match, log=|||||, precedence=3)\n"
"    sni substring: \n"
"      0: .example.com (substring, action=||||match, log=|||||, precedence=3)\n"
"      1:  (all_sites, substring, action=||||match, log=|||||, precedence=3)\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"ip_filter_exact->\n"
"ip_filter_substr->\n"
"all_filter->\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_11)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	opts->user_auth = 1;

	s = strdup("from user root desc desc to cn example.com");
	rv = filter_rule_set(opts, "Divert", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user root desc desc to cn example.com log connect master cert content pcap mirror");
	rv = filter_rule_set(opts, "Split", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = strdup("from user root desc desc to cn example.com log !connect !cert !pcap");
	rv = filter_rule_set(opts, "Pass", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Block action at precedence 2 is not applied to a site of the same rule at precedence 5 now
	s = strdup("from user root desc desc to cn example.com");
	rv = filter_rule_set(opts, "Block", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target
	s = strdup("from user root desc desc to cn example2.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another source
	s = strdup("from user daemon desc desc to cn example.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// all_sites should always be the last element
	s = strdup("from user daemon desc desc to cn *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Search substring (subdomain?)
	s = strdup("from user daemon desc desc to cn .example.com*");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another target
	s = strdup("from user daemon desc desc to cn example3.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another desc
	s = strdup("from user daemon desc desc2 to cn example4.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add all users
	s = strdup("from user * desc desc to cn example5.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add all users all host sites
	s = strdup("from user * desc desc to host *");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	// Add another desc
	s = strdup("from user * desc desc3 to uri example6.com");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 0, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=example.com, port=, ip=, user=root, keyword=desc, exact=site|||user|keyword, all=|||, action=divert||||, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 1: site=example.com, port=, ip=, user=root, keyword=desc, exact=site|||user|keyword, all=|||, action=|split|||, log=connect|master|cert|content|pcap|mirror, apply to=||cn||, precedence=5\n"
		"filter rule 2: site=example.com, port=, ip=, user=root, keyword=desc, exact=site|||user|keyword, all=|||, action=||pass||, log=!connect||!cert||!pcap|, apply to=||cn||, precedence=5\n"
		"filter rule 3: site=example.com, port=, ip=, user=root, keyword=desc, exact=site|||user|keyword, all=|||, action=|||block|, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 4: site=example2.com, port=, ip=, user=root, keyword=desc, exact=site|||user|keyword, all=|||, action=||||match, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 5: site=example.com, port=, ip=, user=daemon, keyword=desc, exact=site|||user|keyword, all=|||, action=||||match, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 6: site=, port=, ip=, user=daemon, keyword=desc, exact=|||user|keyword, all=||sites|, action=||||match, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 7: site=.example.com, port=, ip=, user=daemon, keyword=desc, exact=|||user|keyword, all=|||, action=||||match, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 8: site=example3.com, port=, ip=, user=daemon, keyword=desc, exact=site|||user|keyword, all=|||, action=||||match, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 9: site=example4.com, port=, ip=, user=daemon, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=|||||, apply to=||cn||, precedence=4\n"
		"filter rule 10: site=example5.com, port=, ip=, user=, keyword=desc, exact=site||||keyword, all=|users||, action=||||match, log=|||||, apply to=||cn||, precedence=3\n"
		"filter rule 11: site=, port=, ip=, user=, keyword=desc, exact=||||keyword, all=|users|sites|, action=||||match, log=|||||, apply to=|||host|, precedence=3\n"
		"filter rule 12: site=example6.com, port=, ip=, user=, keyword=desc3, exact=site||||keyword, all=|users||, action=||||match, log=|||||, apply to=||||uri, precedence=3"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);

	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
" user 0 root (exact)=\n"
"  keyword exact:\n"
"   keyword 0 desc (exact)= \n"
"    cn exact: \n"
"      0: example2.com (exact, action=||||match, log=|||||, precedence=4)\n"
"      1: example.com (exact, action=divert|split|pass||, log=!connect|master|!cert|content|!pcap|mirror, precedence=5)\n"
" user 1 daemon (exact)=\n"
"  keyword exact:\n"
"   keyword 0 desc2 (exact)= \n"
"    cn exact: \n"
"      0: example4.com (exact, action=||||match, log=|||||, precedence=4)\n"
"   keyword 1 desc (exact)= \n"
"    cn exact: \n"
"      0: example3.com (exact, action=||||match, log=|||||, precedence=4)\n"
"      1: example.com (exact, action=||||match, log=|||||, precedence=4)\n"
"    cn substring: \n"
"      0: .example.com (substring, action=||||match, log=|||||, precedence=4)\n"
"      1:  (all_sites, substring, action=||||match, log=|||||, precedence=4)\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"   keyword 0 desc3 (exact)= \n"
"    uri exact: \n"
"      0: example6.com (exact, action=||||match, log=|||||, precedence=3)\n"
"   keyword 1 desc (exact)= \n"
"    cn exact: \n"
"      0: example5.com (exact, action=||||match, log=|||||, precedence=3)\n"
"    host substring: \n"
"      0:  (all_sites, substring, action=||||match, log=|||||, precedence=3)\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"ip_filter_exact->\n"
"ip_filter_substr->\n"
"all_filter->\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_12)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("$ips 192.168.0.1 192.168.0.2");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("$dstips 192.168.0.3 192.168.0.4");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("$ports 80 443");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("$logs !master !pcap");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("from ip $ips to ip $dstips port $ports log $logs");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=192.168.0.3, port=80, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 1: site=192.168.0.3, port=80, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 2: site=192.168.0.3, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 3: site=192.168.0.3, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 4: site=192.168.0.4, port=80, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 5: site=192.168.0.4, port=80, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 6: site=192.168.0.4, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 7: site=192.168.0.4, port=443, ip=192.168.0.1, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 8: site=192.168.0.3, port=80, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 9: site=192.168.0.3, port=80, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 10: site=192.168.0.3, port=443, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 11: site=192.168.0.3, port=443, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 12: site=192.168.0.4, port=80, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 13: site=192.168.0.4, port=80, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3\n"
		"filter rule 14: site=192.168.0.4, port=443, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=|!master||||, apply to=dstip||||, precedence=3\n"
		"filter rule 15: site=192.168.0.4, port=443, ip=192.168.0.2, user=, keyword=, exact=site|port|ip||, all=|||, action=||||match, log=||||!pcap|, apply to=dstip||||, precedence=3"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);

	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"ip_filter_exact->\n"
"  ip 0 192.168.0.2 (exact)= \n"
"    ip exact: \n"
"      0: 192.168.0.4 (exact, action=||||, log=|||||, precedence=0)\n"
"        port exact:\n"
"          0: 80 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"          1: 443 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"      1: 192.168.0.3 (exact, action=||||, log=|||||, precedence=0)\n"
"        port exact:\n"
"          0: 80 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"          1: 443 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"  ip 1 192.168.0.1 (exact)= \n"
"    ip exact: \n"
"      0: 192.168.0.4 (exact, action=||||, log=|||||, precedence=0)\n"
"        port exact:\n"
"          0: 80 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"          1: 443 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"      1: 192.168.0.3 (exact, action=||||, log=|||||, precedence=0)\n"
"        port exact:\n"
"          0: 80 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"          1: 443 (exact, action=||||match, log=|!master|||!pcap|, precedence=3)\n"
"ip_filter_substr->\n"
"all_filter->\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

START_TEST(set_filter_rule_13)
{
	char *s;
	int rv;
	opts_t *opts = opts_new();

	s = strdup("$users root daemon");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("$descs desc1 desc2");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	s = strdup("$sites site1 site2");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	// check errors out if we add all log actions to the macro:
	// "../../src/check_pack.c:306: Message string too long"
	// so use 3 log actions only
	// Also, the compiler gives:
	// warning: string length ‘4186’ is greater than the length ‘4095’ ISO C99 compilers are required to support [-Woverlength-strings]
	s = strdup("$logs connect content");
	rv = filter_macro_set(opts, s, 0);
	fail_unless(rv == 0, "failed to set macro");
	free(s);

	opts->user_auth = 1;

	s = strdup("from user $users desc $descs to sni $sites log $logs");
	rv = filter_rule_set(opts, "Match", s, 0);
	fail_unless(rv == 1, "failed to parse rule");
	free(s);

	s = filter_rule_str(opts->filter_rules);
	fail_unless(!strcmp(s,
		"filter rule 0: site=site1, port=, ip=, user=root, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 1: site=site1, port=, ip=, user=root, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 2: site=site2, port=, ip=, user=root, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 3: site=site2, port=, ip=, user=root, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 4: site=site1, port=, ip=, user=root, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 5: site=site1, port=, ip=, user=root, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 6: site=site2, port=, ip=, user=root, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 7: site=site2, port=, ip=, user=root, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 8: site=site1, port=, ip=, user=daemon, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 9: site=site1, port=, ip=, user=daemon, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 10: site=site2, port=, ip=, user=daemon, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 11: site=site2, port=, ip=, user=daemon, keyword=desc1, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 12: site=site1, port=, ip=, user=daemon, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 13: site=site1, port=, ip=, user=daemon, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5\n"
		"filter rule 14: site=site2, port=, ip=, user=daemon, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=connect|||||, apply to=|sni|||, precedence=5\n"
		"filter rule 15: site=site2, port=, ip=, user=daemon, keyword=desc2, exact=site|||user|keyword, all=|||, action=||||match, log=|||content||, apply to=|sni|||, precedence=5"),
		"failed to parse rule: %s", s);	
	free(s);

	close(2);
	opts->filter = filter_set(opts->filter_rules);

	s = filter_str(opts->filter);
	fail_unless(!strcmp(s, "filter=>\n"
"userkeyword_filter_exact->\n"
" user 0 root (exact)=\n"
"  keyword exact:\n"
"   keyword 0 desc2 (exact)= \n"
"    sni exact: \n"
"      0: site2 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"      1: site1 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"   keyword 1 desc1 (exact)= \n"
"    sni exact: \n"
"      0: site2 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"      1: site1 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
" user 1 daemon (exact)=\n"
"  keyword exact:\n"
"   keyword 0 desc2 (exact)= \n"
"    sni exact: \n"
"      0: site2 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"      1: site1 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"   keyword 1 desc1 (exact)= \n"
"    sni exact: \n"
"      0: site2 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"      1: site1 (exact, action=||||match, log=connect|||content||, precedence=5)\n"
"userkeyword_filter_substr->\n"
"user_filter_exact->\n"
"user_filter_substr->\n"
"keyword_filter_exact->\n"
"keyword_filter_substr->\n"
"all_user_filter->\n"
"ip_filter_exact->\n"
"ip_filter_substr->\n"
"all_filter->\n"), "failed to translate rule: %s", s);	
	free(s);

	opts_free(opts);
}
END_TEST

Suite *
filter_suite(void)
{
	Suite *s;
	TCase *tc;
	s = suite_create("filter");

	tc = tcase_create("set_filter_rule");
	tcase_add_test(tc, set_filter_rule_01);
	tcase_add_test(tc, set_filter_rule_02);
	tcase_add_test(tc, set_filter_rule_03);
	tcase_add_test(tc, set_filter_rule_04);
	tcase_add_test(tc, set_filter_rule_05);
	tcase_add_test(tc, set_filter_rule_06);
	tcase_add_test(tc, set_filter_rule_07);
	tcase_add_test(tc, set_filter_rule_08);
	tcase_add_test(tc, set_filter_rule_09);
	tcase_add_test(tc, set_filter_rule_10);
	tcase_add_test(tc, set_filter_rule_11);
	tcase_add_test(tc, set_filter_rule_12);
	tcase_add_test(tc, set_filter_rule_13);
	suite_add_tcase(s, tc);

	return s;
}

/* vim: set noet ft=c: */
