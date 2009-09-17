/* Conspire
 * Copyright (C) 2009 William Pitcock.
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

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdinc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xchat.h"
#include "ignore-ng.h"
#include "cfgfiles.h"
#include "fe.h"
#include "text.h"
#include "modes.h"
#include "util.h"
#include "xchatc.h"
#include "signal_factory.h"
#include "command_factory.h"
#include "command_option.h"

/*
 * Signal handlers for the new ignore system.
 *
 * The idea here is that we halt signal execution if an ignore is in place.
 * As a result, we have to be installed first (using signal_attach_head()).
 *
 * Code that is dependent on these handlers, such as the highlight detection
 * should use signal_attach_after().
 *
 * Any code that wants to receive all signals should be attached *before*
 * this signal, e.g. signal_attach_head().
 */

void
ignore_signal_action_private(gpointer *params)
{
	session *sess     = params[0];
	gchar *from       = params[1];
	struct User *user = userlist_find(sess, from);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PRIVATE | IGNORE_ACTION))
			signal_stop_current();

		g_free(hostmask);
	}
}

void
ignore_signal_action_private_hilight(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *text     = params[2];
	gchar *nickchar = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

                if (ignore_check(hostmask, IGNORE_HILIGHT)) {
			signal_stop_current();
			signal_emit("action private", 5, sess, nick, text, nickchar);
                }

		g_free(hostmask);
        }
}

void
ignore_signal_action_public(gpointer *params)
{
	session *sess     = params[0];
	gchar *from       = params[1];
	struct User *user = userlist_find(sess, from);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PUBLIC | IGNORE_ACTION))
			signal_stop_current();

		g_free(hostmask);
	}
}

void
ignore_signal_action_public_hilight(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *text     = params[2];
	gchar *nickchar = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

                if (ignore_check(hostmask, IGNORE_HILIGHT)) {
			signal_stop_current();
			signal_emit("action public", 5, sess, nick, text, nickchar);
                }

		g_free(hostmask);
        }
}

void
ignore_signal_channel_invited(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[2];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_INVITES))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_channel_topic_changed(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_TOPICS))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_channel_join(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_JOINS))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_channel_kick(gpointer *params)
{
	session *sess  = params[0];
	gchar *kicker  = params[1];
	struct User *user = userlist_find(sess, kicker);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_KICKS))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_channel_modes_raw(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_MODES))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_channel_part(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PARTS))
			signal_stop_current();

		g_free(hostmask);
	}
}

void
ignore_signal_channel_quit(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_QUITS))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_ctcp_inbound(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[2];
	gchar *to     = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (!is_channel(sess->server, to))
		{
			if (ignore_check(hostmask, IGNORE_PRIVATE | IGNORE_CTCP))
				signal_stop_current();
		} else
		{
			if (ignore_check(hostmask, IGNORE_PUBLIC | IGNORE_CTCP))
				signal_stop_current();
		}

		g_free(hostmask);
        }
}

void
ignore_signal_ctcp_reply(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PRIVATE | IGNORE_CTCP))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_dcc_generic(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_DCC))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_message_private(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PRIVATE))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_message_public(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PUBLIC))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_message_public_hilight(gpointer *params)
{
	session *sess   = params[0];
	gchar *nick     = params[1];
	gchar *message  = params[2];
	gchar *nickchar = params[3];
	gchar *idtext   = params[4];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

                if (ignore_check(hostmask, IGNORE_HILIGHT)) {
			signal_stop_current();
			signal_emit("message public", 5, sess, nick, message, nickchar, idtext);
                }

		g_free(hostmask);
        }
}

void
ignore_signal_nick_changed(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	/* XXX - implement nick-following? -- Aerdan */
	/*gchar *newnick = params[2];*/
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_NICKS))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signal_notice_private(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PRIVATE | IGNORE_NOTICE))
			signal_stop_current();

		g_free(hostmask);
	}
}

void
ignore_signal_notice_public(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PUBLIC | IGNORE_NOTICE))
			signal_stop_current();

		g_free(hostmask);
	}
}

void
ignore_signal_query_quit(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PRIVATE | IGNORE_QUITS))
			signal_stop_current();

		g_free(hostmask);
        }
}

void
ignore_signals_init(void)
{
	/* actions */
	signal_attach_head("action private",            ignore_signal_action_private);
	signal_attach_head("action private hilight",    ignore_signal_action_private);
	signal_attach_head("action public",             ignore_signal_action_public);
	signal_attach_head("action public hilight",     ignore_signal_action_public_hilight);

	/* channel events */
	signal_attach_head("channel invited",           ignore_signal_channel_invited);
	signal_attach_head("channel topic changed",     ignore_signal_channel_topic_changed);
	signal_attach_head("channel join",              ignore_signal_channel_join);
	signal_attach_head("channel kick",              ignore_signal_channel_kick);
	signal_attach_head("channel quit",              ignore_signal_channel_quit);
	signal_attach_head("channel modes raw",         ignore_signal_channel_modes_raw);

	/* CTCPs */
	signal_attach_head("ctcp inbound",              ignore_signal_ctcp_inbound);
	signal_attach_head("ctcp reply",                ignore_signal_ctcp_reply);

	/* DCC stuff */
	signal_attach_head("dcc chat duplicate",        ignore_signal_dcc_generic);
	signal_attach_head("dcc chat request",          ignore_signal_dcc_generic);
	signal_attach_head("dcc file request",          ignore_signal_dcc_generic);
	signal_attach_head("dcc file resume",           ignore_signal_dcc_generic);
	signal_attach_head("dcc generic offer",         ignore_signal_dcc_generic);
	signal_attach_head("dcc malformed",             ignore_signal_dcc_generic);

	/* messages */
	signal_attach_head("message private",           ignore_signal_message_private);
	signal_attach_head("message public",            ignore_signal_message_public);
	signal_attach_head("message public hilight",    ignore_signal_message_public_hilight);

	/* nicks */
	signal_attach_head("nick changed",              ignore_signal_nick_changed);

	/* notices */
	signal_attach_head("notice private",            ignore_signal_notice_private);
	signal_attach_head("notice public",             ignore_signal_notice_public);

	/* queries */
	signal_attach_head("query quit",                ignore_signal_query_quit);
}

