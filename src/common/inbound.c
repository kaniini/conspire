/* Conspire
 * Copyright (C) 2007 William Pitcock
 * Portions copyright (c) 2001 Timo Sirainen
 *
 * X-Chat
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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "stdinc.h"
#include <time.h>

#define WANTARPA
#define WANTDNS
#include "inet.h"

#include "xchat.h"
#include "util.h"
#include "fe.h"
#include "modes.h"
#include "notify.h"
#include "outbound.h"
#include "inbound.h"
#include "server.h"
#include "servlist.h"
#include "text.h"
#include "ctcp.h"
#include "xchatc.h"


void
clear_channel (session *sess)
{
	if (sess->channel[0])
		strcpy (sess->waitchannel, sess->channel);
	sess->channel[0] = 0;
	sess->doing_who = FALSE;
	sess->done_away_check = FALSE;

	log_close (sess);

	if (sess->current_modes)
	{
		free (sess->current_modes);
		sess->current_modes = NULL;
	}

	if (sess->mode_timeout_tag)
	{
		g_source_remove (sess->mode_timeout_tag);
		sess->mode_timeout_tag = 0;
	}

	fe_clear_channel (sess);
	userlist_clear (sess);
	fe_set_nonchannel (sess, FALSE);
	fe_set_title (sess);
}

void
set_topic (session *sess, char *topic)
{
	if (sess->topic)
		free (sess->topic);
	sess->topic = strdup (topic);
	fe_set_topic (sess, topic);
}

static session *
find_session_from_nick (char *nick, server *serv)
{
	session *sess;
	GSList *list = sess_list;

	sess = find_dialog (serv, nick);
	if (sess)
		return sess;

	if (serv->front_session)
	{
		if (userlist_find (serv->front_session, nick))
			return serv->front_session;
	}

	if (current_sess && current_sess->server == serv)
	{
		if (userlist_find (current_sess, nick))
			return current_sess;
	}

	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
		{
			if (userlist_find (sess, nick))
				return sess;
		}
		list = list->next;
	}
	return 0;
}

static session *
inbound_open_dialog (server *serv, char *from)
{
	session *sess;

	sess = new_ircwindow (serv, from, SESS_DIALOG, 0);
	signal_emit("query open", 1, sess);

	return sess;
}

static void
inbound_make_idtext (server *serv, char *idtext, int max, int id)
{
	idtext[0] = 0;
	if (serv->have_idmsg)
	{
		if (id && prefs.irc_id_ytext)
		{
			g_strlcpy (idtext, prefs.irc_id_ytext, max);
		} else if (prefs.irc_id_ntext)
		{
			g_strlcpy (idtext, prefs.irc_id_ntext, max);
		}
		/* convert codes like %C,%U to the proper ones */
		check_special_chars (idtext, TRUE);
	}
}

void
inbound_privmsg (server *serv, char *from, char *ip, char *text, int id)
{
	session *sess;
	char idtext[64];

	sess = find_dialog (serv, from);

	if (sess || prefs.autodialog)
	{
		/*0=ctcp  1=priv will set autodialog=0 here is flud detected */
		if (!sess)
		{
			if (flood_check (from, ip, serv, current_sess, 1))
				/* Create a dialog session */
				sess = inbound_open_dialog (serv, from);
			else
				sess = serv->server_session;
		}

		if (prefs.input_beep_priv || (sess && sess->beep))
			sound_beep (sess);

		if (sess && sess->tray)
			fe_tray_set_icon (FE_ICON_MESSAGE);

		if (prefs.input_flash_priv)
			fe_flash_window (sess);

		if (ip && ip[0])
		{
			if (prefs.logging && sess->logfd != -1 &&
				(!sess->topic || strcmp(sess->topic, ip)))
			{
				char tbuf[1024];
				snprintf (tbuf, sizeof (tbuf), "[%s has address %s]\n", from, ip);
				write (sess->logfd, tbuf, strlen (tbuf));
			}
			set_topic (sess, ip);
		}
		inbound_chanmsg (serv, NULL, NULL, from, text, FALSE, id);
		return;
	}

	inbound_make_idtext (serv, idtext, sizeof (idtext), id);

	sess = find_session_from_nick (from, serv);
	if (!sess)
	{
		sess = serv->front_session;

		if (prefs.input_beep_priv || (sess && sess->beep))
			sound_beep (sess);

		signal_emit("message private", 4, sess, from, text, idtext);
		return;
	}

	if (prefs.input_beep_priv || sess->beep)
		sound_beep (sess);

	if (prefs.input_flash_priv)
		fe_flash_window (sess);

	signal_emit("message private", 4, sess, from, text, idtext);
}

static int
SearchNick (char *text, char *nicks)
{
	char S[300];	/* size of irc_extra_hilight in xchatprefs */
	char *n;
	char *p;
	char *t;
	int ns;

	if (nicks == NULL)
		return 0;

	text = strip_color (text, -1, STRIP_ALL);

	g_strlcpy (S, nicks, sizeof (S));
	n = strtok (S, ",");
	while (n != NULL)
	{
		t = text;
		ns = strlen (n);
		while ((p = nocasestrstr (t, n)))
		{
			char *prev_char = (p == text) ? NULL : g_utf8_prev_char (p);
			char *next_char = p + ns;
			if ((!prev_char ||
			     !g_unichar_isalnum (g_utf8_get_char(prev_char))) &&
			    !g_unichar_isalnum (g_utf8_get_char(next_char)))
			{
				free (text);
				return 1;
			}

			t = p + 1;
		}

		n = strtok (NULL, ",");
	}
	free (text);
	return 0;
}

int
FromNick (char *nick, char *nicks)
{
	char S[300];	/* size of irc_no_hilight in xchatprefs */
	char *n;
	char *t;

	if (nicks == NULL || nicks[0] == 0)
		return 0;

	g_strlcpy (S, nicks, sizeof (S));
	n = strtok (S, ",");
	while (n != NULL)
	{
		t = nick;
		if (nocasestrstr(t, n))
			return 1;
		n = strtok (NULL, ",");
	}
	return 0;
}

static int
is_hilight (char *from, char *text, session *sess, server *serv)
{
	if (FromNick(from, prefs.irc_no_hilight))
		return 0;

	if (SearchNick (text, serv->nick) ||
		 SearchNick (text, prefs.irc_extra_hilight) ||
		 FromNick (from, prefs.irc_nick_hilight))
	{
		if (sess != current_tab)
		{
			sess->nick_said = TRUE;
			lastact_update(sess);
		}
		fe_set_hilight (sess);
		return 1;
	}
	return 0;
}

void
inbound_action (session *sess, char *chan, char *from, char *text, int fromme, int id)
{
	session *def = sess;
	server *serv = sess->server;
	int beep = FALSE;
	struct User *user;
	int hilight = FALSE;
	char nickchar[2] = "\000";

	if (!fromme)
	{
		if (is_channel (serv, chan))
		{
			sess = find_channel (serv, chan);
			beep = prefs.input_beep_chans;
		} else
		{
			/* it's a private action! */
			beep = prefs.input_beep_priv;
			/* find a dialog tab for it */
			sess = find_dialog (serv, from);
			/* if non found, open a new one */
			if (!sess && prefs.autodialog)
				sess = inbound_open_dialog (serv, from);
		}
	}

	if (!sess)
		sess = def;

	if (sess != current_tab)
	{
		if (fromme)
		{
			sess->msg_said = FALSE;
			sess->new_data = TRUE;
		} else
		{
			sess->msg_said = TRUE;
			sess->new_data = FALSE;
		}
		lastact_update(sess);
	}

	user = userlist_find (sess, from);
	if (user)
	{
		nickchar[0] = user->prefix[0];
		user->lasttalk = time (0);
	}

	if (!fromme)
	{
		hilight = is_hilight (from, text, sess, serv);

		if (!prefs.hilight_enable)
			hilight = FALSE;

		if (hilight && prefs.input_beep_hilight)
			beep = TRUE;

		if (beep || sess->beep)
			sound_beep (sess);

		if (sess->tray)
			fe_tray_set_icon (FE_ICON_MESSAGE);

		/* private action, flash? */
		if (!is_channel (serv, chan) && prefs.input_flash_priv)
		{
			fe_flash_window (sess);
			signal_emit("action private hilight", 4, sess, from, text, nickchar);
			return;
                } else if (hilight)
		{
			signal_emit("action public hilight", 4, sess, from, text, nickchar);
			return;
		}
	}

	if (fromme)
		signal_emit("user action", 4, sess, from, text, nickchar);
	else if (is_channel(serv, chan))
		signal_emit("action public", 4, sess, from, text, nickchar);
	else
		signal_emit("action private", 4, sess, from, text, nickchar);
}

void
inbound_chanmsg (server *serv, session *sess, char *chan, char *from, char *text, char fromme, int id)
{
	struct User *user;
	char nickchar[2] = "\000";
	char idtext[64];

	if (!sess)
	{
		if (chan)
		{
			sess = find_channel (serv, chan);
			if (!sess && !is_channel (serv, chan))
				sess = find_dialog (serv, chan);
		} else
		{
			sess = find_dialog (serv, from);
		}
		if (!sess)
			return;
	}

	if (sess != current_tab)
	{
		sess->msg_said = TRUE;
		sess->new_data = FALSE;
		lastact_update(sess);
	}

	user = userlist_find (sess, from);
	if (user)
	{
		nickchar[0] = user->prefix[0];
		user->lasttalk = time (0);
	}

	if (fromme)
	{
  		if (prefs.auto_unmark_away && serv->is_away)
			sess->server->p_set_back (sess->server);
		signal_emit("user message public", 4, sess, from, text, nickchar);
		return;
	}

	inbound_make_idtext (serv, idtext, sizeof (idtext), id);

	if (sess->type != SESS_DIALOG)
	{
		if (prefs.input_beep_chans || sess->beep)
			sound_beep (sess);

		if (sess->tray)
			fe_tray_set_icon (FE_ICON_MESSAGE);
	}

	if (prefs.hilight_enable)
        {
		if (is_hilight (from, text, sess, serv))
		{
			if (prefs.input_beep_hilight)
				sound_beep (sess);
			signal_emit("message public hilight", 5, sess, from, text, nickchar, idtext);
        	        return;
		}
		else
		{
			if (sess->type != SESS_DIALOG && prefs.input_flash_chans)
				fe_flash_window (sess);
		}
        }

	if (sess->type == SESS_DIALOG)
		signal_emit("message private", 4, sess, from, text, idtext);
	else
		signal_emit("message public", 5, sess, from, text, nickchar, idtext);
}

void
inbound_newnick (server *serv, char *nick, char *newnick, int quiet)
{
	int me = FALSE;
	session *sess;
	GSList *list = sess_list;

	if (!serv->p_cmp (nick, serv->nick))
	{
		me = TRUE;
		g_strlcpy (serv->nick, newnick, NICKLEN);
	}

	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
		{
			if (userlist_change (sess, nick, newnick) || (me && sess->type == SESS_SERVER))
			{
				if (!quiet)
				{
					if (me)
						signal_emit("user nick changed", 3, sess, nick, newnick);
					else
						signal_emit("nick changed", 3, sess, nick, newnick);
				}
			}
			if (sess->type == SESS_DIALOG && !serv->p_cmp (sess->channel, nick))
			{
				g_strlcpy (sess->channel, newnick, CHANLEN);
				fe_set_channel (sess);
			}
			fe_set_title (sess);
		}
		list = list->next;
	}

	dcc_change_nick (serv, nick, newnick);

	if (me)
		fe_set_nick (serv, newnick);
}

/* find a "<none>" tab */
static session *
find_unused_session (server *serv)
{
	session *sess;
	GSList *list = sess_list;
	while (list)
	{
		sess = (session *) list->data;
		if (sess->type == SESS_CHANNEL && sess->channel[0] == 0 &&
			 sess->server == serv)
		{
			if (sess->waitchannel[0] == 0)
				return sess;
		}
		list = list->next;
	}
	return 0;
}

static session *
find_session_from_waitchannel (char *chan, struct server *serv)
{
	session *sess;
	GSList *list = sess_list;
	while (list)
	{
		sess = (session *) list->data;
		if (sess->server == serv && sess->channel[0] == 0 && sess->type == SESS_CHANNEL)
		{
			if (!serv->p_cmp (chan, sess->waitchannel))
				return sess;
		}
		list = list->next;
	}
	return 0;
}

void
inbound_ujoin (server *serv, char *chan, char *nick, char *ip)
{
	session *sess;

	/* already joined? probably a bnc */
	sess = find_channel (serv, chan);
	if (!sess)
	{
		/* see if a window is waiting to join this channel */
		sess = find_session_from_waitchannel (chan, serv);
		if (!sess)
		{
			/* find a "<none>" tab and use that */
			sess = find_unused_session (serv);
			if (!sess)
				/* last resort, open a new tab/window */
				sess = new_ircwindow (serv, chan, SESS_CHANNEL, 1);
		}
	}

	g_strlcpy (sess->channel, chan, CHANLEN);

	fe_set_channel (sess);
	fe_set_title (sess);
	fe_set_nonchannel (sess, TRUE);
	userlist_clear (sess);

	if (prefs.logging)
		log_open (sess);

	sess->waitchannel[0] = 0;
	sess->ignore_date = TRUE;
	sess->ignore_mode = TRUE;
	sess->ignore_names = TRUE;
	sess->end_of_names = FALSE;

	/* sends a MODE */
	serv->p_join_info (sess->server, chan);

	signal_emit("user joined", 4, sess, nick, chan, ip);

	if (prefs.userhost)
	{
		/* sends WHO #channel */
		serv->p_user_list (sess->server, chan);
		sess->doing_who = TRUE;
	}
}

void
inbound_ukick (server *serv, char *chan, char *kicker, char *reason)
{
	session *sess = find_channel (serv, chan);
	if (sess)
	{
		signal_emit("user kicked", 5, sess, serv->nick, chan, kicker, reason);
		clear_channel (sess);
		if (prefs.autorejoin)
		{
			serv->p_join (serv, chan, sess->channelkey);
			g_strlcpy (sess->waitchannel, chan, CHANLEN);
		}
	}
}

void
inbound_upart (server *serv, char *chan, char *ip, char *reason)
{
	session *sess = find_channel (serv, chan);
	if (sess)
	{
		signal_emit("user part", 5, sess, serv->nick, ip, chan, reason);
		clear_channel (sess);
	}
}

void
inbound_nameslist (server *serv, char *chan, char *names)
{
	session *sess;
	char name[NICKLEN];
	int pos = 0;

	sess = find_channel (serv, chan);
	if (!sess)
	{
		signal_emit("channel users", 3, serv->server_session, chan, names);
		return;
	}
	if (!sess->ignore_names)
		signal_emit("channel users", 3, sess, chan, names);

	if (sess->end_of_names)
	{
		sess->end_of_names = FALSE;
		userlist_clear (sess);
		fe_userlist_numbers_block(sess);
	}

	while (1)
	{
		switch (*names)
		{
		case 0:
			name[pos] = 0;
			if (pos != 0)
				userlist_add (sess, name, 0);
			return;
		case ' ':
			name[pos] = 0;
			pos = 0;
			userlist_add (sess, name, 0);
			break;
		default:
			name[pos] = *names;
			if (pos < (NICKLEN-1))
				pos++;
		}
		names++;
	}
}

void
inbound_topic (server *serv, char *chan, char *topic_text)
{
	session *sess = find_channel (serv, chan);
	char *new_topic;

	if (sess)
	{
		new_topic = strip_color (topic_text, -1, STRIP_ALL);
		set_topic (sess, new_topic);
		free (new_topic);
	} else
		sess = serv->server_session;

	signal_emit("channel topic", 3, sess, chan, topic_text);
}

void
inbound_topicnew (server *serv, char *nick, char *chan, char *topic)
{
	session *sess;
	char *new_topic;

	sess = find_channel (serv, chan);
	if (sess)
	{
		new_topic = strip_color (topic, -1, STRIP_ALL);
		set_topic (sess, new_topic);
		free (new_topic);
		signal_emit("channel topic changed", 4, sess, nick, topic, chan);
	}
}

void
inbound_join (server *serv, char *chan, char *user, char *ip)
{
	session *sess = find_channel (serv, chan);
	if (sess)
	{
		if (!sess->hide_join_part)
			signal_emit("channel join", 4, sess, user, chan, ip);
		userlist_add (sess, user, ip);
	}
}

void
inbound_kick (server *serv, char *chan, char *user, char *kicker, char *reason)
{
	session *sess = find_channel (serv, chan);
	if (sess)
	{
		signal_emit("channel kick", 5, sess, kicker, user, chan, reason);
		userlist_remove (sess, user);
	}
}

void
inbound_part (server *serv, char *chan, char *user, char *ip, char *reason)
{
	session *sess = find_channel (serv, chan);
	if (sess)
	{
		if (!sess->hide_join_part)
		{
			signal_emit("channel part", 5, sess, user, ip, chan, reason);
		}
		userlist_remove (sess, user);
	}
}

void
inbound_topictime (server *serv, char *chan, char *nick, time_t stamp)
{
	char *tim = ctime (&stamp);
	session *sess = find_channel (serv, chan);

	if (!sess)
		sess = serv->server_session;

	tim[24] = 0;	/* get rid of the \n */
	signal_emit("channel topic date", 4, sess, chan, nick, tim);
}

static gboolean
netsplit_display_victims(server *serv)
{
	GString *buffer;
	GSList *head;
	GSList *list = sess_list;
	session *sess;

	for (; list != NULL; list = list->next)
	{
		sess = (session *) list->data;
		if (sess->server == serv && sess->type == SESS_CHANNEL)
		{
			buffer = g_string_new("");

			/* tab was affected by netsplit, attempt to display victims for this tab */
			for (head = sess->split_list; head != NULL; head = head->next)
			{
				if (buffer->len < 420)
				{
					g_string_append_printf(buffer, "%s%s",
						*buffer->str != '\0' ? ", " : "",
						(gchar *) head->data);
				}
				else
				{
					signal_emit("server netsplit", 3, sess, serv, buffer->str);
					g_string_erase(buffer, 0, -1);
				}
				g_free(head->data);
			}

			if (buffer->len)
				signal_emit("server netsplit", 3, sess, serv, buffer->str);

			/* free the list for this window */
			g_slist_free(sess->split_list);
			sess->split_list = NULL;

			/* and clear buffer for next one */
			g_string_free(buffer, TRUE);
		}
	}

	g_free(serv->split_reason);
	g_free(serv->split_serv1);
	g_free(serv->split_serv2);
	serv->split_reason = NULL;
	serv->split_serv1 = NULL;
	serv->split_serv2 = NULL;
	serv->split_timer = 0;
	return FALSE;
}

/* check if quit message is a netsplit message -- shamelessly
   stolen from irssi. --nenolod */
static gboolean
quitmsg_is_split(const char *msg)
{
	const char *host1, *host2, *p;
        int prev, len, host1_dot, host2_dot;

	if (msg == NULL)
		return FALSE;

	/* NOTE: there used to be some paranoia checks (some older IRC
	   clients have even more), but they're pretty useless nowadays,
	   since IRC server prefixes the quit message with a space if it
	   looks like a netsplit message.

	   So, the check is currently just:
             - host1.domain1 host2.domain2
             - top-level domains have to be 2+ characters long,
	       containing only alphabets
	     - only 1 space
	     - no double-dots (".." - probably useless check)
	     - hosts/domains can't start or end with a dot
             - the two hosts can't be identical (probably useless check)
	     - can't contain ':' or '/' chars (some servers allow URLs)
	   */
	host1 = msg; host2 = NULL;
	prev = '\0'; len = 0; host1_dot = host2_dot = 0;
	while (*msg != '\0') {
		if (*msg == ' ') {
			if (prev == '.' || prev == '\0') {
				/* domains can't end with '.', space can't
				   be the first character in msg. */
				return FALSE;
			}
			if (host2 != NULL)
				return FALSE; /* only one space allowed */
			if (!host1_dot)
                                return FALSE; /* host1 didn't have domain */
                        host2 = msg+1; len = -1;
		} else if (*msg == '.') {
			if (prev == '\0' || prev == ' ' || prev == '.') {
				/* domains can't start with '.'
				   and can't have ".." */
				return FALSE;
			}

			if (host2 != NULL)
				host2_dot = TRUE;
			else
                                host1_dot = TRUE;
		} else if (*msg == ':' || *msg == '/')
			return FALSE;

		prev = *msg;
                msg++; len++;
	}

	if (!host2_dot || prev == '.')
                return FALSE;

        /* top-domain1 must be 2+ chars long and contain only alphabets */
	p = host2-1;
	while (p[-1] != '.') {
		if (!isalpha(p[-1]))
                        return FALSE;
		p--;
	}
	if (host2-p-1 < 2) return FALSE;

        /* top-domain2 must be 2+ chars long and contain only alphabets */
	p = host2+strlen(host2);
	while (p[-1] != '.') {
		if (!isalpha(p[-1]))
                        return FALSE;
		p--;
	}
	if (strlen(p) < 2) return FALSE;

        return TRUE;
}

void
inbound_quit (server *serv, char *nick, char *ip, char *reason)
{
	GSList *list = sess_list;
	session *sess;
	int was_on_front_session = FALSE;
	gboolean netsplit = FALSE;

	if (serv->split_reason && !strcmp(serv->split_reason, reason))
		netsplit = TRUE;
	else if ((netsplit = quitmsg_is_split(reason)) == TRUE)
	{
		if (netsplit)
		{
			if (serv->split_reason)
			{
				if (serv->split_timer)
					g_source_remove(serv->split_timer);
				netsplit_display_victims(serv);
			}
			else
			{
				gchar *seperator = strchr(reason, ' ');

				if (seperator)
				{
					*seperator = '\0';
					serv->split_serv1 = g_strdup(reason);
					serv->split_serv2 = g_strdup(seperator + 1);
					*seperator = ' ';
				}

				serv->split_reason = g_strdup(reason);
			}
		}
	}

	if (!netsplit && prefs.strip_quits)
	{
		/* strip IRCd Quit: prefix */
		char *tmp;

		if (((tmp = strstr(reason, "Quit:")) != NULL) ||
		    ((tmp = strstr(reason, "Exit:")) != NULL))
			reason = tmp + 6; /* skip past Quit: */
	}

	for (; list != NULL; list = list->next)
	{
		sess = (session *) list->data;
		if (sess->server == serv)
		{
 			if (sess == current_sess)
 				was_on_front_session = TRUE;
			if (userlist_remove (sess, nick))
			{
				if (netsplit)
				{
					sess->split_list = g_slist_append(sess->split_list, g_strdup(nick));

					if (serv->split_timer)
						g_source_remove(serv->split_timer);

					serv->split_timer = g_timeout_add(500, (GSourceFunc) netsplit_display_victims, serv);
				}
				else if (!sess->hide_join_part)
				{
					signal_emit("channel quit", 4, sess, nick, reason, ip);
				}
			}
			else if (sess->type == SESS_DIALOG &&
				!serv->p_cmp (sess->channel, nick))
			{
				/* previously, this wasn't displayed for dialog sessions, I think it's a good idea. :) */
				signal_emit("query quit", 4, sess, nick, reason, ip);
			}
		}
	}

	notify_set_offline (serv, nick, was_on_front_session);
}

void
inbound_ping_reply (session *sess, char *timestring, char *from)
{
	unsigned long tim, nowtim, dif;
	int lag = 0;
	char outbuf[64];

	if (strncmp (timestring, "LAG", 3) == 0)
	{
		timestring += 3;
		lag = 1;
	}

	tim = strtoul (timestring, NULL, 10);
	nowtim = make_ping_time ();
	dif = nowtim - tim;

	sess->server->ping_recv = time (0);

	if (lag)
	{
		sess->server->lag_sent = 0;
		sess->server->lag = dif / 1000;
		fe_set_lag (sess->server, dif / 100000);
		return;
	}

	if (atol (timestring) == 0)
	{
		if (sess->server->lag_sent)
			sess->server->lag_sent = 0;
		else
			signal_emit("server ping reply", 3, sess, from, "?");
	} else
	{
		snprintf (outbuf, sizeof (outbuf), "%ld.%ld%ld", dif / 1000000, (dif / 100000) % 10, dif % 10);
		signal_emit("server ping reply", 3, sess, from, outbuf);
	}
}

static session *
find_session_from_type (int type, server *serv)
{
	session *sess;
	GSList *list = sess_list;
	while (list)
	{
		sess = list->data;
		if (sess->type == type && serv == sess->server)
			return sess;
		list = list->next;
	}
	return 0;
}

void
inbound_ctcp_reply (struct session *sess, char *msg, char *nick)
{
	char *temp = g_strdup(msg);
	char *type = strtok(temp, " ");
	short len = strlen(type);
	msg[strlen(msg)-1] = 0;

	signal_emit("ctcp reply", 4, sess, nick, type, msg+(len+1));

	g_free(temp);
}

void
inbound_notice (server *serv, char *to, char *nick, char *msg, char *ip, int id)
{
	char *po,*ptr=to;
	session *sess = 0;
	int server_notice = FALSE;

	if (is_channel (serv, ptr))
		sess = find_channel (serv, ptr);

	if (!sess && ptr[0] == '@')
	{
		ptr++;
		sess = find_channel (serv, ptr);
	}

	if (!sess && ptr[0] == '%')
	{
		ptr++;
		sess = find_channel (serv, ptr);
	}

	if (!sess && ptr[0] == '+')
	{
		ptr++;
		sess = find_channel (serv, ptr);
	}

	if (strcmp (nick, ip) == 0)
		server_notice = TRUE;

	if (!sess)
	{
		ptr = 0;
		if (prefs.notices_tabs)
		{
			int stype = server_notice ? SESS_SNOTICES : SESS_NOTICES;
			sess = find_session_from_type (stype, serv);
			if (!sess)
			{
				if (stype == SESS_NOTICES)
					sess = new_ircwindow (serv, "(notices)", SESS_NOTICES, 0);
				else
					sess = new_ircwindow (serv, "(snotices)", SESS_SNOTICES, 0);
				fe_set_channel (sess);
				fe_set_title (sess);
				fe_set_nonchannel (sess, FALSE);
				userlist_clear (sess);
				if (prefs.logging)
					log_open (sess);
			}
			/* Avoid redundancy with some Undernet notices */
			if (!strncmp (msg, "*** Notice -- ", 14))
				msg += 14;
		} else
		{
											/* paranoia check */
			if (msg[0] == '[' && (!serv->have_idmsg || id))
			{
				/* guess where chanserv meant to post this -sigh- */
				if (!strcasecmp (nick, "ChanServ") && !find_dialog (serv, nick))
				{
					char *dest = strdup (msg + 1);
					char *end = strchr (dest, ']');
					if (end)
					{
						*end = 0;
						sess = find_channel (serv, dest);
					}
					free (dest);
				}
			}
			if (!sess)
				sess = find_session_from_nick (nick, serv);
		}
		if (!sess)
		{
			if (server_notice)
				sess = serv->server_session;
			else
				sess = serv->front_session;
		}
	}

	if (msg[0] == 1)
	{
		msg++;
		if (!strncmp (msg, "PING", 4))
		{
			inbound_ping_reply (sess, msg + 5, nick);
			return;
		} else {
			inbound_ctcp_reply(sess, msg, nick);
			return;
		}
	}
	po = strchr (msg, '\001');
	if (po)
		po[0] = 0;

	if (server_notice)
		signal_emit("server notice", 3, sess, msg, nick);
	else if (ptr)
		signal_emit("notice public", 4, sess, nick, to, msg);
	else
		signal_emit("notice private", 3, sess, nick, msg);
}

void
inbound_away (server *serv, char *nick, char *msg)
{
	struct away_msg *away = server_away_find_message (serv, nick);
	session *sess = NULL;
	GSList *list;

	if (away && !strcmp (msg, away->message))	/* Seen the msg before? */
	{
		if (prefs.show_away_once && !serv->inside_whois)
			return;
	} else
	{
		server_away_save_message (serv, nick, msg);
	}

	sess = serv->front_session;

	signal_emit("whois away", 3, sess, nick, msg);

	list = sess_list;
	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
			userlist_set_away (sess, nick, TRUE);
		list = list->next;
	}
}

int
inbound_nameslist_end (server *serv, char *chan)
{
	session *sess;
	GSList *list;

	if (!strcmp (chan, "*"))
	{
		list = sess_list;
		while (list)
		{
			sess = list->data;
			if (sess->server == serv)
			{
				sess->end_of_names = TRUE;
				sess->ignore_names = FALSE;

				fe_userlist_numbers_unblock(sess);
				fe_userlist_numbers(sess);
			}
			list = list->next;
		}
		return TRUE;
	}
	sess = find_channel (serv, chan);
	if (sess)
	{
		sess->end_of_names = TRUE;
		sess->ignore_names = FALSE;
		return TRUE;
	}
	return FALSE;
}

static gboolean
check_willjoin_channels (server *serv)
{
	char *po;
	session *sess;
	GSList *list = sess_list;
	int i = 0;

	/* shouldnt really happen, the io tag is destroyed in server.c */
	if (!is_server (serv))
		return FALSE;

	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
		{
			if (sess->willjoinchannel[0] != 0)
			{
				strcpy (sess->waitchannel, sess->willjoinchannel);
				sess->willjoinchannel[0] = 0;
				serv->p_join (serv, sess->waitchannel, sess->channelkey);
				po = strchr (sess->waitchannel, ',');
				if (po)
					*po = 0;
				po = strchr (sess->waitchannel, ' ');
				if (po)
					*po = 0;
				i++;
			}
		}
		list = list->next;
	}
	serv->joindelay_tag = 0;
	fe_server_event (serv, FE_SE_LOGGEDIN, i);
	return FALSE;
}

void
inbound_next_nick (session *sess, char *nick)
{
	char *newnick;
	server *serv = sess->server;
	ircnet *net;

	serv->nickcount++;

	switch (serv->nickcount)
	{
	case 2:
		newnick = g_strdup(prefs.nick2);
		net = serv->network;
		/* use network specific "Second choice"? */
		if (net && !(net->flags & FLAG_USE_GLOBAL) && net->nick2)
			newnick = g_strdup(net->nick2);
		serv->p_change_nick (serv, newnick);
		signal_emit("nick clash", 3, sess, nick, newnick);
		break;

	case 3:
		newnick = g_strdup(prefs.nick3);
		serv->p_change_nick (serv, prefs.nick3);
		signal_emit("nick clash", 3, sess, nick, newnick);
		break;

	default:
		signal_emit("nick error", 1, sess);
	}
}

static void
set_default_modes (server *serv)
{
	char modes[8];

	modes[0] = '+';
	modes[1] = '\0';

	if (prefs.wallops)
		strcat (modes, "w");
	if (prefs.servernotice)
		strcat (modes, "s");
	if (prefs.invisible)
		strcat (modes, "i");

	if (modes[1] != '\0')
	{
		serv->p_mode (serv, serv->nick, modes);
	}
}

void
inbound_login_start (session *sess, char *nick, char *servname)
{
	inbound_newnick (sess->server, sess->server->nick, nick, TRUE);
	server_set_name (sess->server, servname);
	if (sess->type == SESS_SERVER && prefs.logging)
		log_open (sess);
	/* reset our away status */
	if (sess->server->reconnect_away)
	{
		handle_command (sess->server->server_session, "away", FALSE);
		sess->server->reconnect_away = FALSE;
	}
}

static void
inbound_set_all_away_status (server *serv, char *nick, unsigned int status)
{
	GSList *list;
	session *sess;

	list = sess_list;
	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
			userlist_set_away (sess, nick, status);
		list = list->next;
	}
}

void
inbound_uaway (server *serv)
{
	serv->is_away = TRUE;
	serv->away_time = time (NULL);
	fe_set_away (serv);

	inbound_set_all_away_status (serv, serv->nick, 1);
}

void
inbound_uback (server *serv)
{
	serv->is_away = FALSE;
	serv->reconnect_away = FALSE;
	fe_set_away (serv);

	inbound_set_all_away_status (serv, serv->nick, 0);
}

void
inbound_foundip (session *sess, char *ip)
{
	struct hostent *HostAddr;

	HostAddr = gethostbyname (ip);
	if (HostAddr)
		prefs.dcc_ip = ((struct in_addr *) HostAddr->h_addr)->s_addr;

	signal_emit("server found ip", 2, sess, ip);
}

void
inbound_user_info_start (session *sess, char *nick)
{
	/* set away to FALSE now, 301 may turn it back on */
	inbound_set_all_away_status (sess->server, nick, 0);
}

int
inbound_user_info (session *sess, char *chan, char *user, char *host,
						 char *servname, char *nick, char *realname,
						 unsigned int away)
{
	server *serv = sess->server;
	session *who_sess;
	char *uhost;

	who_sess = find_channel (serv, chan);
	if (who_sess)
	{
		if (user && host)
		{
			uhost = malloc (strlen (user) + strlen (host) + 2);
			sprintf (uhost, "%s@%s", user, host);
			if (!userlist_add_hostname (who_sess, nick, uhost, realname, servname, away))
			{
				if (!who_sess->doing_who)
				{
					free (uhost);
					return 0;
				}
			}
			free (uhost);
		} else
		{
			if (!userlist_add_hostname (who_sess, nick, NULL, realname, servname, away))
			{
				if (!who_sess->doing_who)
					return 0;
			}
		}
	}
	return 1;
}

int
inbound_banlist (session *sess, time_t stamp, char *chan, char *mask, char *banner, int is_exemption)
{
	char *time_str = ctime (&stamp);
	server *serv = sess->server;

	time_str[19] = 0;	/* get rid of the \n */
	if (stamp == 0)
		time_str = "";

	sess = find_channel (serv, chan);
	if (!sess)
	{
		sess = serv->front_session;
		goto nowindow;
	}

   if (!fe_is_banwindow (sess))
	{
nowindow:
		/* let proto-irc.c do the 'goto def' for exemptions */
		if (is_exemption)
			return FALSE;

		signal_emit("channel bans", 5, sess, chan, mask, banner, time_str);
		return TRUE;
	}

	fe_add_ban_list (sess, mask, banner, time_str, is_exemption);
	return TRUE;
}

/* execute 1 end-of-motd command */

static int
inbound_exec_eom_cmd (char *str, void *sess)
{
	handle_command (sess, (str[0] == '/') ? str + 1 : str, TRUE);
	return 1;
}

void
inbound_login_end (session *sess, char *text)
{
	server *serv = sess->server;

	if (!serv->end_of_motd)
	{
		if (prefs.ip_from_server && serv->use_who)
		{
			serv->skip_next_userhost = TRUE;
			serv->p_get_ip_uh (serv, serv->nick);	/* sends USERHOST mynick */
		}
		set_default_modes (serv);

		if (serv->network)
		{
			/* there may be more than 1, separated by \n */
			if (((ircnet *)serv->network)->command)
				token_foreach (((ircnet *)serv->network)->command, '\n',
									inbound_exec_eom_cmd, sess);

			/* send nickserv password */
			if (((ircnet *)serv->network)->nickserv)
				serv->p_ns_identify (serv, ((ircnet *)serv->network)->nickserv);
		}

		/* send JOIN now or wait? */
		if (serv->network && ((ircnet *)serv->network)->nickserv &&
			 prefs.irc_join_delay)
			serv->joindelay_tag = g_timeout_add (prefs.irc_join_delay * 1000, (GSourceFunc) check_willjoin_channels, serv);
		else
			check_willjoin_channels (serv);
		if (serv->supports_watch)
			notify_send_watches (serv);
		else if(serv->supports_monitor)
			notify_send_monitor (serv);

		serv->end_of_motd = TRUE;
	}
	if (prefs.skipmotd && !serv->motd_skipped)
	{
		serv->motd_skipped = TRUE;
		return;
	}
        signal_emit("server motd", 2, serv->server_session, text);
}

void
inbound_identified (server *serv)	/* 'MODE +e MYSELF' on freenode */
{
	if (serv->joindelay_tag)
	{
		/* stop waiting, just auto JOIN now */
		g_source_remove (serv->joindelay_tag);
		serv->joindelay_tag = 0;
		check_willjoin_channels (serv);
	}
}
