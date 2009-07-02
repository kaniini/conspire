/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
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

#include <stdio.h>
#include <string.h>
#include "stdinc.h"
#include <stdlib.h>

#include <time.h>

#include "xchat.h"
#include "cfgfiles.h"
#include "util.h"
#include "modes.h"
#include "outbound.h"
#include "ignore-ng.h"
#include "inbound.h"
#include "dcc.h"
#include "text.h"
#include "ctcp.h"
#include "server.h"
#include "userlist.h"
#include "xchatc.h"


static void
ctcp_reply (session *sess, char *nick, char *word[], char *word_eol[],
				char *conf)
{
	char tbuf[4096];	/* can receive 2048 from IRC, so this is enough */

	conf = strdup (conf);
	/* process %C %B etc */
	check_special_chars (conf, TRUE);
	auto_insert (tbuf, sizeof (tbuf), conf, word, word_eol, "", "", word_eol[5],
					 server_get_network (sess->server, TRUE), "", "", nick);
	free (conf);
	handle_command (sess, tbuf, FALSE);
}

static int
ctcp_check (session *sess, char *nick, char *word[], char *word_eol[],
				char *ctcp)
{
	int ret = 0;
	char *po;
	struct popup *pop;
	GSList *list = ctcp_list;

	po = strchr (ctcp, '\001');
	if (po)
		*po = 0;

	po = strchr (word_eol[5], '\001');
	if (po)
		*po = 0;

	while (list)
	{
		pop = (struct popup *) list->data;
		if (!strcasecmp (ctcp, pop->name))
		{
			ctcp_reply (sess, nick, word, word_eol, pop->cmd);
			ret = 1;
		}
		list = list->next;
	}
	return ret;
}

void
ctcp_handle (session *sess, char *to, char *nick, char *msg, char *word[], char *word_eol[], int id)
{
	char *po;
	server *serv = sess->server;
	char outbuf[1024];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask = g_strjoin("!", user->nick, user->hostname, NULL);
	gboolean channel = is_channel(serv, to);
	IgnoreLevel level = (channel) ? IGNORE_PUBLIC : IGNORE_PRIVATE;
        level |= IGNORE_CTCP;

	/* consider DCC to be different from other CTCPs */
	if (!g_ascii_strncasecmp (msg, "DCC", 3))
	{
		/* but still let CTCP replies override it */
		if (!ctcp_check (sess, nick, word, word_eol, word[4] + 2))
		{
			if (!ignore_check (hostmask, IGNORE_DCC))
				handle_dcc (sess, nick, word, word_eol);
		}
		return;
	}

	/* consider ACTION to be different from other CTCPs. Check
      ignore as if it was a PRIV/CHAN. */
	if (!g_ascii_strncasecmp (msg, "ACTION ", 7))
	{
		/* but still let CTCP replies override it */
		if (ctcp_check (sess, nick, word, word_eol, word[4] + 2))
			goto generic;

		inbound_action (sess, to, nick, msg + 7, FALSE, id);
		return;
	}

	if (!ignore_check(hostmask, level)) {
		if (!strcasecmp (msg, "VERSION") && !prefs.hidever)
		{
#ifndef _WIN32
			struct utsname un;

			uname(&un);

			snprintf (outbuf, sizeof (outbuf), "VERSION conspire "PACKAGE_VERSION" - running on %s %s %s",
						 un.sysname, un.release, un.machine);
#else
			OSVERSIONINFO osvi;
			SYSTEM_INFO si;

			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&osvi);
			GetSystemInfo(&si);

			snprintf(outbuf, sizeof(outbuf), "VERSION conspire "PACKAGE_VERSION" - running on Windows %ld.%ld build %ld (i%d86)",
				osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber,
				si.wProcessorLevel);
#endif

			serv->p_nctcp (serv, nick, outbuf);
		} else if (!strcasecmp(msg, "CLIENTINFO"))
		{
			snprintf(outbuf, sizeof(outbuf), "CLIENTINFO CLIENTINFO PING TIME USERINFO VERSION");

			serv->p_nctcp(serv, nick, outbuf);
		} else if (!strcasecmp(msg, "USERINFO"))
		{
			snprintf(outbuf, sizeof(outbuf), "USERINFO %s", prefs.realname);

			serv->p_nctcp(serv, nick, outbuf);
		} else if (!strcasecmp(msg, "TIME")) {
			time_t time_val = time(NULL);
			struct tm *tval = localtime(&time_val);
			char tbuf[200];
			strftime(tbuf, sizeof(tbuf), prefs.irc_time_format, tval); //Sun Feb  3 18:33:27 CST 2008
			snprintf(outbuf, sizeof(outbuf), "TIME %s", tbuf);

			serv->p_nctcp(serv, nick, outbuf);
		}
        }

generic:
	po = strchr (msg, '\001');
	if (po)
		po[0] = 0;

	signal_emit("ctcp inbound", 4, sess, msg, nick, to);
}
