/* Conspire
 * Copyright (C) 2008 William Pitcock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "xchat.h"
#include "ctcp.h"
#include "fe.h"
#include "ignore.h"
#include "inbound.h"
#include "modes.h"
#include "notify.h"
#include "server.h"
#include "text.h"
#include "outbound.h"
#include "util.h"
#include "xchatc.h"
#include "base64.h"
#include "sasl.h"

static void
process_authenticate(gpointer *params)
{
	session *sess = params[0];
	gchar **word_eol = params[2];
	gchar *buf = params[3];
	server *serv = sess->server;
	sess = serv->server_session;

	if (*word_eol[2] == '+')
	{
		gchar buf[1024];
		gchar b64buf[1024];
		gchar *iter_p = buf;
		gsize ret;

		ret = g_strlcpy(iter_p, serv->sasl_user, 1024 - (iter_p - buf));
		iter_p += ret + 1;
		ret = g_strlcpy(iter_p, serv->sasl_user, 1024 - (iter_p - buf));
		iter_p += ret + 1;
		ret = g_strlcpy(iter_p, serv->sasl_pass, 1024 - (iter_p - buf));

		base64_encode(buf, (strlen(serv->sasl_user) * 2) + strlen(serv->sasl_pass) + 2, b64buf, 1024);

		/* TODO: chunk this in 400 byte increments */
		tcp_sendf(serv, "AUTHENTICATE %s\r\n", b64buf);
	}
	else if (!word_eol[2])
	{
		g_source_remove(serv->sasl_timeout_tag);
		tcp_sendf(serv, "AUTHENTICATE *\r\n");
		tcp_sendf(serv, "CAP END\r\n");
		serv->sasl_state = SASL_COMPLETE;
		return;
	}
}

void
sasl_init(void)
{
	signal_attach("server message authenticate", process_authenticate);
}
