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
	gchar *text       = params[2];
	gchar *nickchar   = params[3];
	struct User *user = userlist_find(sess, from);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PRIVATE | IGNORE_ACTION))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
	}
}

void
ignore_signal_action_public(gpointer *params)
{
	session *sess     = params[0];
	gchar *from       = params[1];
	gchar *text       = params[2];
	gchar *nickchar   = params[3];
	struct User *user = userlist_find(sess, from);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PUBLIC | IGNORE_ACTION))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
	}
}

void
ignore_signal_channel_invited(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar *nick   = params[2];
	server *serv  = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_INVITES))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
        }
}

void
ignore_signal_channel_topic_changed(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *topic   = params[2];
	gchar *channel = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_TOPICS))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
        }
}

void
ignore_signal_channel_join(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *channel = params[2];
	gchar *host    = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_JOINS))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
        }
}

void
ignore_signal_channel_kick(gpointer *params)
{
	session *sess  = params[0];
	gchar *kicker  = params[1];
	gchar *nick    = params[2];
	gchar *channel = params[3];
	gchar *reason  = params[4];
	struct User *user = userlist_find(sess, kicker);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_KICKS))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
        }
}

void
ignore_signal_channel_part(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *host    = params[2];
	gchar *channel = params[3];
	gchar *reason  = params[4];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_PARTS))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
	}
}

void
ignore_signal_channel_quit(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *reason = params[2];
	gchar *host   = params[3];
	struct User *user = userlist_find(sess, nick);
	gchar *hostmask;

	if (user != NULL)
	{
		hostmask = g_strjoin("!", user->nick, user->hostname, NULL);

		if (ignore_check(hostmask, IGNORE_QUITS))
			signal_stop(signal_get_current_name());

		g_free(hostmask);
        }
}

void
ignore_signals_init(void)
{
	/* actions */
	signal_attach_head("action private",		ignore_signal_action_private);
	signal_attach_head("action public",		ignore_signal_action_public);

	/* channel events */
	signal_attach_head("channel invited",		ignore_signal_channel_invited);
	signal_attach_head("channel topic changed",	ignore_signal_channel_topic_changed);
	signal_attach_head("channel join",		ignore_signal_channel_join);
	signal_attach_head("channel kick",		ignore_signal_channel_kick);
	signal_attach_head("channel quit",		ignore_signal_channel_quit);
}
