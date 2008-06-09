/* X-Chat
 * Copyright (C) 2002 Peter Zelezny.
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

/* IRC RFC1459(+commonly used extensions) protocol implementation */

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

static void
irc_login (server *serv, char *user, char *realname)
{
	if (serv->password[0])
		tcp_sendf (serv, "PASS %s\r\n", serv->password);

	/* redo this in conspire 0.11 ;) --nenolod */
	if (serv->sasl_user && serv->sasl_pass)
		tcp_sendf (serv, "CAP REQ :sasl multi-prefix\r\n");
	else
		tcp_sendf (serv, "CAP REQ :multi-prefix\r\n");

	tcp_sendf (serv,
				  "NICK %s\r\n"
				  "USER %s %s %s :%s\r\n",
				  serv->nick, user, user, serv->servername, realname);

	serv->sasl_state = SASL_INITIALIZED;
}

static void
irc_nickserv (server *serv, char *cmd, char *arg1, char *arg2, char *arg3)
{
	switch (serv->nickservtype)
	{
	case 0:
		tcp_sendf (serv, "PRIVMSG NICKSERV :%s %s%s%s\r\n", cmd, arg1, arg2, arg3);
		break;
	case 1:
		tcp_sendf (serv, "NICKSERV %s %s%s%s\r\n", cmd, arg1, arg2, arg3);
		break;
	case 2:
		tcp_sendf (serv, "NS %s %s%s%s\r\n", cmd, arg1, arg2, arg3);
		break;
	case 3:
		tcp_sendf (serv, "PRIVMSG NS :%s %s%s%s\r\n", cmd, arg1, arg2, arg3);
	}
}

static void
irc_ns_identify (server *serv, char *pass)
{
	irc_nickserv (serv, "IDENTIFY", pass, "", "");
}

static void
irc_ns_ghost (server *serv, char *usname, char *pass)
{
	irc_nickserv (serv, "GHOST", usname, " ", pass);
}

static void
irc_join (server *serv, char *channel, char *key)
{
	if (key[0])
		tcp_sendf (serv, "JOIN %s %s\r\n", channel, key);
	else
		tcp_sendf (serv, "JOIN %s\r\n", channel);
}

static void
irc_part (server *serv, char *channel, char *reason)
{
	if (reason[0])
		tcp_sendf (serv, "PART %s :%s\r\n", channel, reason);
	else
		tcp_sendf (serv, "PART %s\r\n", channel);
}

static void
irc_quit (server *serv, char *reason)
{
	if (reason[0])
		tcp_sendf (serv, "QUIT :%s\r\n", reason);
	else
		tcp_send_len (serv, "QUIT\r\n", 6);
}

static void
irc_set_back (server *serv)
{
	tcp_send_len (serv, "AWAY\r\n", 6);
}

static void
irc_set_away (server *serv, char *reason)
{
	tcp_sendf (serv, "AWAY :%s\r\n", reason);
}

static void
irc_ctcp (server *serv, char *to, char *msg)
{
	tcp_sendf (serv, "PRIVMSG %s :\001%s\001\r\n", to, msg);
}

static void
irc_nctcp (server *serv, char *to, char *msg)
{
	tcp_sendf (serv, "NOTICE %s :\001%s\001\r\n", to, msg);
}

static void
irc_cycle (server *serv, char *channel, char *key)
{
	tcp_sendf (serv, "PART %s\r\nJOIN %s %s\r\n", channel, channel, key);
}

static void
irc_kick (server *serv, char *channel, char *nick, char *reason)
{
	if (reason[0])
		tcp_sendf (serv, "KICK %s %s :%s\r\n", channel, nick, reason);
	else
		tcp_sendf (serv, "KICK %s %s\r\n", channel, nick);
}

static void
irc_invite (server *serv, char *channel, char *nick)
{
	tcp_sendf (serv, "INVITE %s %s\r\n", nick, channel);
}

static void
irc_mode (server *serv, char *target, char *mode)
{
	tcp_sendf (serv, "MODE %s %s\r\n", target, mode);
}

/* find channel info when joined */

static void
irc_join_info (server *serv, char *channel)
{
	tcp_sendf (serv, "MODE %s\r\n", channel);
}

/* initiate userlist retreival */

static void
irc_user_list (server *serv, char *channel)
{
	tcp_sendf (serv, "WHO %s\r\n", channel);
}

/* userhost */

static void
irc_userhost (server *serv, char *nick)
{
	tcp_sendf (serv, "USERHOST %s\r\n", nick);
}

static void
irc_away_status (server *serv, char *channel)
{
	if (serv->have_whox)
		tcp_sendf (serv, "WHO %s %%ctnf,152\r\n", channel);
	else
		tcp_sendf (serv, "WHO %s\r\n", channel);
}

/*static void
irc_get_ip (server *serv, char *nick)
{
	tcp_sendf (serv, "WHO %s\r\n", nick);
}*/


/*
 *  Command: WHOIS
 *     Parameters: [<server>] <nickmask>[,<nickmask>[,...]]
 */
static void
irc_user_whois (server *serv, char *nicks)
{
	tcp_sendf (serv, "WHOIS %s\r\n", nicks);
}

static void
irc_message (server *serv, char *channel, char *text)
{
	tcp_sendf (serv, "PRIVMSG %s :%s\r\n", channel, text);
}

static void
irc_action (server *serv, char *channel, char *act)
{
	tcp_sendf (serv, "PRIVMSG %s :\001ACTION %s\001\r\n", channel, act);
}

static void
irc_notice (server *serv, char *channel, char *text)
{
	tcp_sendf (serv, "NOTICE %s :%s\r\n", channel, text);
}

static void
irc_topic (server *serv, char *channel, char *topic)
{
	if (!topic)
		tcp_sendf (serv, "TOPIC %s :\r\n", channel);
	else if (topic[0])
		tcp_sendf (serv, "TOPIC %s :%s\r\n", channel, topic);
	else
		tcp_sendf (serv, "TOPIC %s\r\n", channel);
}

static void
irc_list_channels (server *serv, char *arg, int min_users)
{
	if (arg[0])
	{
		tcp_sendf (serv, "LIST %s\r\n", arg);
		return;
	}

	if (serv->use_listargs)
		tcp_sendf (serv, "LIST >%d,<10000\r\n", min_users - 1);
	else
		tcp_send_len (serv, "LIST\r\n", 6);
}

static void
irc_names (server *serv, char *channel)
{
	tcp_sendf (serv, "NAMES %s\r\n", channel);
}

static void
irc_change_nick (server *serv, char *new_nick)
{
	tcp_sendf (serv, "NICK %s\r\n", new_nick);
}

static void
irc_ping (server *serv, char *to, char *timestring)
{
	if (*to)
		tcp_sendf (serv, "PRIVMSG %s :\001PING %s\001\r\n", to, timestring);
	else
		tcp_sendf (serv, "PING %s\r\n", timestring);
}

static int
irc_raw (server *serv, char *raw)
{
	int len;
	char tbuf[4096];
	if (*raw)
	{
		len = strlen (raw);
		if (len < sizeof (tbuf) - 3)
		{
			len = snprintf (tbuf, sizeof (tbuf), "%s\r\n", raw);
			tcp_send_len (serv, tbuf, len);
		} else
		{
			tcp_send_len (serv, raw, len);
			tcp_send_len (serv, "\r\n", 2);
		}
		return TRUE;
	}
	return FALSE;
}

/* ============================================================== */
/* ======================= IRC INPUT ============================ */
/* ============================================================== */


static void
channel_date (session *sess, char *chan, char *timestr)
{
	time_t timestamp = (time_t) atol (timestr);
	char *tim = ctime (&timestamp);
	tim[24] = 0;	/* get rid of the \n */
	signal_emit("channel created", 3, sess, chan, tim);
}

static void
server_text_passthrough(server *serv, char **word, char *text)
{
	if (is_channel (serv, word[4]))
	{
		session *realsess = find_channel (serv, word[4]);
		if (!realsess)
			realsess = serv->server_session;
		signal_emit("server text", 3, realsess, text, word[1]);
	}
	else
	{
		signal_emit("server text", 3, serv->server_session, text, word[1]);
	}
}

/* giant ugly hackaround */
static void
process_numeric_001 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	inbound_login_start (sess, word[3], word[1]);

	/* umm .. what the fuck is all this shit? we should just set a services agent
	   in the network factory .. --nenolod */

	/* use /NICKSERV */
	if (strcasecmp (word[7], "DALnet") == 0 ||
		 strcasecmp (word[7], "BRASnet") == 0)
		serv->nickservtype = 1;

	/* use /NS */
	else if (strcasecmp (word[7], "FreeNode") == 0)
		serv->nickservtype = 2;
}

static void
process_numeric_004 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	serv->use_listargs = FALSE;

	/* default to IRC RFC */
	serv->modes_per_line = 3;

	/* DALnet */
	if (strncmp (word[5], "bahamut", 7) == 0)
		/* use the /list args */
		serv->use_listargs = TRUE;

	/* Undernet */
	else if (strncmp (word[5], "u2.10.", 6) == 0)
	{
		/* use the /list args */
		serv->use_listargs = TRUE;
		/* allow 6 modes per line */
		serv->modes_per_line = 6;
	}
	else if (strncmp (word[5], "glx2", 4) == 0)
	{
		/* use the /list args */
		serv->use_listargs = TRUE;
	}

	server_text_passthrough(serv, word, text);
}

static void
process_numeric_005 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	inbound_005(serv, word);

	server_text_passthrough(serv, word, text);
}

static void
process_numeric_263 (gpointer *params)
{
	/* Load too high (RPL_TRYAGAIN) */
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (fe_is_chanwindow (serv))
		fe_chan_list_end (serv);

	server_text_passthrough(serv, word, text);
}

static void
process_numeric_290 (gpointer *params)
{
	/* CAPAB reply */
	session *sess = params[0];
	char ** word = params[1];
	char **word_eol = params[2];
	char *text = params[3];
	server *serv = sess->server;

	if (strstr(word_eol[1], "IDENTIFY-MSG"))
	{
		serv->have_idmsg = TRUE;
		return;
	}

	server_text_passthrough(serv, word, text);
}

static void
process_numeric_301 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	inbound_away(serv, word[4], (word_eol[5][0] == ':') ? word_eol[5] + 1 : word_eol[5]);
}

static void
process_numeric_302 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (serv->skip_next_userhost)
	{
		char *eq = strchr(word[4], '=');
		if (eq)
		{
			*eq = 0;
			if (!serv->p_cmp(word[4] + 1, serv->nick))
			{
				char *at = strrchr(eq + 1, '@');
				if (at)
					inbound_foundip(sess, at + 1);
			}
		}

		serv->skip_next_userhost = FALSE;
		return;
	}
	else
		server_text_passthrough(serv, word, text);
}

static void
process_numeric_303 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	word[4]++;
	notify_markonline(serv, word);
}

static void
process_numeric_305 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	inbound_uback(serv);

	server_text_passthrough(serv, word, text);
}

static void
process_numeric_306 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	inbound_uaway(serv);

	server_text_passthrough(serv, word, text);
}

static void
process_numeric_312 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	signal_emit("whois server", 3, whois_sess, word[4], word_eol[5]);
}

/* handles 311 (RPL_WHOISUSER) and 314 (RPL_WHOWASUSER) */
static void
process_whois_user (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	inbound_user_info_start(sess, word[4]);
	signal_emit("whois name", 3, whois_sess, word, word_eol);
}

static void
process_numeric_311 (gpointer *params)
{
	session *sess = params[0];
	server *serv = sess->server;

	serv->inside_whois = 1;
}

static void
process_numeric_313 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	signal_emit("whois oper", 3, whois_sess, word, word_eol);
}

/* RPL_ENDOFWHO */
static void
process_numeric_315 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	session *who_sess;
	who_sess = find_channel(serv, word[4]);
	if (who_sess)
	{
		if (!who_sess->doing_who)
			signal_emit("server text", 3, serv->server_session, text, word[1]);
		who_sess->doing_who = FALSE;
	}
	else
	{
		if (!serv->doing_dns)
			signal_emit("server text", 3, serv->server_session, text, word[1]);
		serv->doing_dns = FALSE;
	}
}

static void
process_numeric_317 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	time_t timestamp = (time_t) atol(word[6]);
	long idle = atol(word[5]);
	char *tim;
	char outbuf[64];

	snprintf(outbuf, sizeof (outbuf),
		"%02ld:%02ld:%02ld", idle / 3600, (idle / 60) % 60, idle % 60);

	if (timestamp == 0)
		signal_emit("whois idle", 3, whois_sess, word[4], outbuf);
	else
	{
		tim = ctime(&timestamp);
		/* get rid of the \n */
		tim[19] = 0;
		signal_emit("whois idle signon", 4, whois_sess, word[4], outbuf, tim);
	}
}

static void
process_numeric_318 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	serv->inside_whois = 0;
	signal_emit("whois end", 2, whois_sess, word[4]);
}

static void
process_numeric_319 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	signal_emit("whois channels", 3, whois_sess, word, word_eol);
}

/* this comes in several forms depending on the IRCd, notably 307
   (RPL_WHOISREGNICK, bahamut, Unreal) and 320 (RPL_WHOISSPECIAL ??).
   figure out how to attach signals to this based on IRCd. -- nfontes */
static void
process_whois_identified (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	signal_emit("whois identified", 3, whois_sess, word, word_eol);
}

static void
process_numeric_321 (gpointer *params)
{
	session *sess = params[0];
	server *serv = sess->server;

	if (!fe_is_chanwindow(serv))
		signal_emit("channel list head", 1, serv);
}

static void
process_numeric_322 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	if (!fe_is_chanwindow(serv))
		signal_emit("channel list entry", 3, sess, word, word_eol);
	else
		fe_add_chan_list(serv, word[4], word[5], word_eol[6] + 1);
}

static void
process_numeric_323 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (!fe_is_chanwindow(serv))
		signal_emit("server text", 3, serv, text, word[1]);
	else
		fe_chan_list_end(serv);
}

static void
process_numeric_324 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	sess = find_channel(serv, word[4]);
	if (!sess)
		sess = serv->server_session;

	if (sess->ignore_mode)
		sess->ignore_mode = FALSE;
	else
		signal_emit("channel modes", 3, sess, word, word_eol);

	fe_update_mode_buttons (sess, 't', '-');
	fe_update_mode_buttons (sess, 'n', '-');
	fe_update_mode_buttons (sess, 's', '-');
	fe_update_mode_buttons (sess, 'i', '-');
	fe_update_mode_buttons (sess, 'p', '-');
	fe_update_mode_buttons (sess, 'm', '-');
	fe_update_mode_buttons (sess, 'l', '-');
	fe_update_mode_buttons (sess, 'k', '-');
	handle_mode(serv, word, word_eol, "", TRUE);
}

static void
process_numeric_329 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	sess = find_channel(serv, word[4]);
	if (sess)
	{
		if (sess->ignore_date)
			sess->ignore_date = FALSE;
		else
			channel_date(sess, word[4], word[5]);
	}
}

static void
process_numeric_330 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	signal_emit("whois authenticated", 3, whois_sess, word, word_eol);
}

static void
process_numeric_332 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	inbound_topic(serv, word[4], (word_eol[5][0] == ':') ? word_eol[5] + 1 : word_eol[5]);
}

static void
process_numeric_333 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	inbound_topictime(serv, word[4], word[5], atol(word[6]));
}

/* RPL_INVITING */
static void
process_numeric_341 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	signal_emit("user invite", 3, sess, word, serv);
}

/* RPL_EXCEPTLIST */
static void
process_numeric_348 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (!inbound_banlist(sess, atol(word[7]), word[4], word[5], word[6], TRUE))
		server_text_passthrough(serv, word, text);
}

/* RPL_ENDOFEXCEPTLIST */
static void
process_numeric_349 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	sess = find_channel(serv, word[4]);
	if (!sess)
	{
		sess = serv->front_session;
		server_text_passthrough(serv, word, text);
		return;
	}

	if (!fe_is_banwindow (sess))
		server_text_passthrough(serv, word, text);
	else
		fe_ban_list_end(sess, TRUE);
}

/* RPL_WHOREPLY */
static void
process_numeric_352 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	char *text = params[3];
	server *serv = sess->server;

	gboolean away = FALSE;
	session *who_sess = find_channel(serv, word[4]);

	if (*word[9] == 'G')
		away = TRUE;

	inbound_user_info(sess, word[4], word[5], word[6], word[7],
		word[8], word_eol[11], away);

	/* try to show only user initiated whos */
	if (!who_sess || !who_sess->doing_who)
		signal_emit("server text", 3, serv->server_session, text, word[1]);
}

/* RPL_NAMREPLY */
static void
process_numeric_353 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	inbound_nameslist(serv, word[5], (word_eol[6][0] == ':') ? word_eol[6] + 1 : word_eol[6]);
}

/* RPL_WHOSPCRPL -- Undernet WHOX */
static void
process_numeric_354 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	gboolean away = FALSE;
	session *who_sess;

	/* irc_away_status sends out a "152" */
	if (strcmp(word[4], "152") == 0)
	{
		who_sess = find_channel(serv, word[5]);

		if (*word[7] == 'G')
			away = TRUE;

		/* :SanJose.CA.us.undernet.org 354 z1 152 #zed1 z1 H@ */
		inbound_user_info(sess, word[5], 0, 0, 0, word[6], 0, away);

		/* try to show only user initiated whos */
		if (!who_sess || !who_sess->doing_who)
			signal_emit("server text", 3, serv->server_session, text, word[1]);
	}
	else
		server_text_passthrough(serv, word, text);
}

static void
process_numeric_366 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (!inbound_nameslist_end(serv, word[4]))
		server_text_passthrough(serv, word, text);
}

/* RPL_BANLIST */
static void
process_numeric_367 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];

	inbound_banlist(sess, atol(word[7]), word[4], word[5], word[6], FALSE);
}

static void
process_numeric_368 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	sess = find_channel(serv, word[4]);
	if (!sess)
	{
		sess = serv->front_session;
		server_text_passthrough(serv, word, text);
		return;
	}

	if (!fe_is_banwindow(sess))
		server_text_passthrough(serv, word, text);
	else
		fe_ban_list_end(sess, FALSE);
}

/* 369 (RPL_ENDOFWHOWAS) and 406 (ERR_WASNOSUCHNICK) */
static void
process_whowas_end (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	signal_emit("server text", 3, whois_sess, text, word[1]);
	serv->inside_whois = 0;
}

/* 372 (RPL_MOTD) and 375 (RPL_MOTDSTART) */
static void
process_motd (gpointer *params)
{
	session *sess = params[0];
	char *text = params[3];
	server *serv = sess->server;

	if (!prefs.skipmotd || serv->motd_skipped)
		signal_emit("server motd", 2, serv->server_session, text);
}

/* 376 (RPL_ENDOFMOTD) and 422 (ERR_NOMOTD) */
static void
process_motd_end (gpointer *params)
{
	session *sess = params[0];
	char *text = params[3];

	inbound_login_end (sess, text);
}

/* 432 (ERR_ERRONEUSNICKNAME) and 433 (ERR_NICKNAMEINUSE) */
static void
process_nickname_change_error (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (serv->end_of_motd)
		server_text_passthrough(serv, word, text);
	else
		inbound_next_nick(sess, word[4]);
}

static void
process_numeric_437 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	if (serv->end_of_motd || is_channel(serv, word[4]))
		server_text_passthrough(serv, word, text);
	else
		inbound_next_nick(sess, word[4]);
}

static void
process_numeric_471 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];

	signal_emit("channel join error", 3, sess, word[4], _("user limit reached"));
}

static void
process_numeric_473 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];

	signal_emit("channel join error", 3, sess, word[4], _("channel is invite-only"));
}

static void
process_numeric_474 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];

	signal_emit("channel join error", 3, sess, word[4], _("you are banned"));
}

static void
process_numeric_475 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];

	signal_emit("channel join error", 3, sess, word[4], _("channel requires a keyword"));
}

/* 600 (RPL_LOGON) and 604 (RPL_NOWON) (Bahamut, Unreal) */
static void
process_notify_online (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	notify_set_online (serv, word[4]);
}

/* RPL_LOGOFF (Bahamut, Unreal) */
static void
process_numeric_601 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	notify_set_offline (serv, word[4], FALSE);
}

/* RPL_NOWOFF (Bahamut, Unreal) */
static void
process_numeric_605 (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	notify_set_offline (serv, word[4], TRUE);
}

static void
process_monitor_reply (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char *text = params[3];
	server *serv = sess->server;

	int n = atoi(word[2]);

	switch(n)
	{
	case 730:
	case 731:
		{
			char *nick, *p;
			for(nick = strtok(word[4] +1, ","); nick != NULL; nick = strtok(NULL, ","))
			{
				p = strchr(nick, '!');
				if(p != NULL)
					*p = '\0';
				if(n == 731)
					notify_set_offline(serv, nick, serv->inside_monitor);
				else
					notify_set_online(serv, nick);
			}
		}
		break;

	case 732:
		if(!serv->inside_monitor)
			server_text_passthrough(serv, word, text);
		break;
	case 733:
		if(serv->inside_monitor)
			serv->inside_monitor = FALSE;
		break;
	default:
		break;
	}
}

static void
process_numeric (gpointer *params)
{
	session *sess = params[0];
	int n = GPOINTER_TO_INT(params[1]);
	char **word = params[2];
	char **word_eol = params[3];
	char *text = params[4];
	server *serv = sess->server;
	session *whois_sess = serv->front_session;

	switch (n)
	{
	default:
		if (serv->inside_whois && word[4][0])
		{
			/* some unknown WHOIS reply, ircd coders make them up weekly */
			signal_emit("whois generic", 3, whois_sess, word, word_eol);
			return;
		}

	/*def:*/

		if (is_channel (serv, word[4]))
		{
			session *realsess = find_channel (serv, word[4]);
			if (!realsess)
				realsess = serv->server_session;
			signal_emit("server text", 3, realsess, text, word[1]);
		} else
		{
			signal_emit("server text", 3, serv->server_session, text, word[1]);
		}
	}
}

/* TODO get nick, user, hostname processing into their own functions! */

static void
process_peer_cap (gpointer *params)
{
	session *sess = params[0];
	char **word_eol = params[2];
	server *serv = sess->server;

	serv->cap = cap_state_new(serv, word_eol[5]);

	signal_emit("cap message", 1, serv->cap);

	/* end CAP negotiation if nothing has locked state. --nenolod */
	cap_state_unref(serv->cap);
}

static void
process_peer_invite (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		/* this is rigged. disgraceful. */
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	if (ignore_check(word[1], IG_INVI))
		return;

	signal_emit("channel invited", 4, sess, word, nick, serv);
}

static void
process_peer_join (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	char ip[128], nick[NICKLEN], *ex;
	/* fill in the "ip" and "nick" buffers */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(ip, word[1], sizeof (ip));
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		g_strlcpy(ip, ex + 1, sizeof (ip));
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	char *chan = word[3];

	if (*chan == ':')
		chan++;

	if (!serv->p_cmp(nick, serv->nick))
		inbound_ujoin(serv, chan, nick, ip);
	else
		inbound_join(serv, chan, nick, ip);
}

static void
process_peer_kick (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	char *kicked = word[4];
	char *reason = word_eol[5];
	if (*kicked)
	{
		if (*reason == ':')
			reason++;

		if (strcmp(kicked, serv->nick) == 0)
			inbound_ukick(serv, word[3], nick, reason);
		else
			inbound_kick(serv, word[3], kicked, nick, reason);
	}
}

static void
process_peer_kill (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	signal_emit("server kill", 3, sess, nick, word_eol);
}

static void
process_peer_mode (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	handle_mode (serv, word, word_eol, nick, FALSE);	/* modes.c */
}

static void
process_peer_nick (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	inbound_newnick (serv, nick, (word_eol[3][0] == ':') ? word_eol[3] + 1 : word_eol[3], FALSE);
}

static void
process_peer_notice (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	char *text;
	server *serv = sess->server;
	gboolean id; /* freenode crap? */

	char ip[128], nick[NICKLEN], *ex;
	/* fill in the "ip" and "nick" buffers */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(ip, word[1], sizeof (ip));
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		g_strlcpy(ip, ex + 1, sizeof (ip));
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	id = FALSE;	/* identified */

	text = word_eol[4];
	if (*text == ':')
		text++;

	/* freenode crap? */
	if (serv->have_idmsg)
	{
		if (*text == '+')
		{
			id = TRUE;
			text++;
		} else if (*text == '-')
			text++;
	}

	if (!ignore_check(word[1], IG_NOTI))
		inbound_notice(serv, word[3], nick, text, ip, id);
}

static void
process_peer_part (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	char ip[128], nick[NICKLEN], *ex;
	/* fill in the "ip" and "nick" buffers */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(ip, word[1], sizeof (ip));
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		g_strlcpy(ip, ex + 1, sizeof (ip));
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	char *chan = word[3];
	char *reason = word_eol[4];

	if (*chan == ':')
		chan++;

	if (*reason == ':')
		reason++;

	if (strcmp(nick, serv->nick) == 0)
		inbound_upart (serv, chan, ip, reason);
	else
		inbound_part (serv, chan, nick, ip, reason);
}

static void
process_peer_pong (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	server *serv = sess->server;

	inbound_ping_reply(serv->server_session, (word[4][0] == ':') ? word[4] + 1 : word[4], word[3]);
}

static void
process_peer_privmsg (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	char *text;
	server *serv = sess->server;
	char *to;
	int len;
	gboolean id; /* freenode crap? */

	char ip[128], nick[NICKLEN], *ex;
	/* fill in the "ip" and "nick" buffers */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(ip, word[1], sizeof (ip));
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		g_strlcpy(ip, ex + 1, sizeof (ip));
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	to = word[3];
	id = FALSE;
	if (*to)
	{
		text = word_eol[4];
		if (*text == ':')
			text++;

		/* freenode crap? */
		if (serv->have_idmsg)
		{
			if (*text == '+')
			{
				id = TRUE;
				text++;
			} else if (*text == '-')
				text++;
		}

		len = strlen(text);

		/* CTCP? */
		if (*text == '\1' && text[len - 1] == 1)
		{
			/* needs signals ... */
			text[len - 1] = '\0';
			text++;
			if (strncasecmp (text, "ACTION", 6) != 0)
				flood_check (nick, ip, serv, sess, 0);
			if (strncasecmp (text, "DCC ", 4) == 0)
				/* redo this with handle_quotes TRUE */
				process_data_init (word[1], word_eol[1], word, word_eol, TRUE, FALSE);
			ctcp_handle (sess, to, nick, text, word, word_eol, id);
		}
		else
		{
			if (is_channel(serv, to))
			{
				if (ignore_check(word[1], IG_CHAN))
					return;
				inbound_chanmsg(serv, NULL, to, nick, text, FALSE, id);
			}
			else
			{
				if (ignore_check(word[1], IG_PRIV))
					return;
				inbound_privmsg(serv, nick, ip, text, id);
			}
		}
	}
}

static void
process_peer_quit (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	char ip[128], nick[NICKLEN], *ex;
	/* fill in the "ip" and "nick" buffers */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(ip, word[1], sizeof (ip));
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		g_strlcpy(ip, ex + 1, sizeof (ip));
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	inbound_quit(serv, nick, ip, (word_eol[3][0] == ':') ? word_eol[3] + 1 : word_eol[3]);
}

static void
process_peer_topic (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	server *serv = sess->server;

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	inbound_topicnew(serv, nick, word[3], (word_eol[4][0] == ':') ? word_eol[4] + 1 : word_eol[4]);
}

static void
process_peer_wallops (gpointer *params)
{
	session *sess = params[0];
	char **word = params[1];
	char **word_eol = params[2];
	char *text;

	char nick[NICKLEN], *ex;
	/* fill in the "nick" buffer */
	ex = strchr(word[1], '!');
	if (!ex)							  /* no '!', must be a server message */
	{
		g_strlcpy(nick, word[1], sizeof (nick));
	} else
	{
		ex[0] = 0;
		g_strlcpy(nick, word[1], sizeof (nick));
		ex[0] = '!';
	}

	text = word_eol[3];
	if (*text == ':')
		text++;

	signal_emit("server wallops", 3, sess, nick, text);
}

static void
process_named_msg (gpointer *params)
{
	session *sess = params[0];
	char **word_eol = params[3];

	/* unknown message */
	PrintTextf (sess, "Unknown message: %s\n", word_eol[1]);
}

/* handle named messages that DON'T start with a ':' */

static void
process_message_error (gpointer *params)
{
	session *sess = params[0];
	char **word_eol = params[2];
	char *text;

	text = word_eol[2];
	if (*text == ':')
		text++;

	signal_emit("server error", 2, sess, text);
}

static void
process_message_notice (gpointer *params)
{
	session *sess = params[0];
	char **word_eol = params[2];
	char *text;

	text = word_eol[3];
	if (*text == ':')
		text++;

	signal_emit("server notice", 2, sess, text);
}

static void
process_message_ping (gpointer *params)
{
	session *sess = params[0];
	char **word_eol = params[2];
	server *serv = sess->server;

	tcp_sendf(serv, "PONG %s\r\n", word_eol[2]);
}

static void
process_named_servermsg(gpointer *params)
{
	session *sess = params[0];
	char *buf = params[4];
	server *serv = sess->server;
	sess = serv->server_session;

	signal_emit("server text", 3, sess->server, buf, sess->server->servername);
}

/* irc_inline() - 1 single line received from serv */

static void
irc_inline (server *serv, char *buf, int len)
{
	session *sess, *tmp;
	char *type, *text;
	char *word[PDIWORDS];
	char *word_eol[PDIWORDS];
	char pdibuf_static[522]; /* 1 line can potentially be 512*6 in utf8 */
	char *pdibuf = pdibuf_static;

	/* need more than 522? fall back to malloc */
	if (len >= sizeof (pdibuf_static))
		pdibuf = malloc (len + 1);

	sess = serv->front_session;

	if (buf[0] == ':')
	{
		/* split line into words and words_to_end_of_line */
		process_data_init (pdibuf, buf, word, word_eol, FALSE, FALSE);

		/* find a context for this message */
		if (is_channel (serv, word[3]))
		{
			tmp = find_channel (serv, word[3]);
			if (tmp)
				sess = tmp;
		}

		/* for server messages, the 2nd word is the "message type" */
		type = word[2];

		word[0] = type;
		word[1]++;
		word_eol[1] = buf + 1;	/* but not for xchat internally */

	}
	else
	{
		process_data_init (pdibuf, buf, word, word_eol, FALSE, FALSE);
		word[0] = type = word[1];
	}

	if (buf[0] != ':')
	{
		static gchar scratch[512];
		gint sigs;

		g_snprintf(scratch, 512, "server message %s", word[1]);
		sigs = signal_emit(scratch, 4, sess, word, word_eol, buf);

		if (!sigs)
			signal_emit("server message", 5, sess, word[1], word, word_eol, buf);

		goto xit;
	}

	static gchar scratch[512];
	gint sigs;
	/* see if the second word is a numeric */
	if (isdigit((unsigned char) word[2][0]))
	{
		text = word_eol[4];
		if (*text == ':')
			text++;

		g_snprintf(scratch, 512, "server numeric %s", word[2]);
		sigs = signal_emit(scratch, 4, sess, word, word_eol, text);

		if (!sigs)
			signal_emit("server numeric", 5, sess, atoi(word[2]), word, word_eol, text);
	}
	else
	{
		/* text isn't necessarily word_eol[4] here... */
		g_snprintf(scratch, 512, "server peer %s", word[2]);
		sigs = signal_emit(scratch, 3, sess, word, word_eol);

		if (!sigs)
			signal_emit("server peer", 4, sess, word[2], word, word_eol);
	}

xit:
	if (pdibuf != pdibuf_static)
		free (pdibuf);
}

void
proto_fill_her_up (server *serv)
{
	serv->p_inline = irc_inline;
	serv->p_invite = irc_invite;
	serv->p_cycle = irc_cycle;
	serv->p_ctcp = irc_ctcp;
	serv->p_nctcp = irc_nctcp;
	serv->p_quit = irc_quit;
	serv->p_kick = irc_kick;
	serv->p_part = irc_part;
	serv->p_ns_identify = irc_ns_identify;
	serv->p_ns_ghost = irc_ns_ghost;
	serv->p_join = irc_join;
	serv->p_login = irc_login;
	serv->p_join_info = irc_join_info;
	serv->p_mode = irc_mode;
	serv->p_user_list = irc_user_list;
	serv->p_away_status = irc_away_status;
	/*serv->p_get_ip = irc_get_ip;*/
	serv->p_whois = irc_user_whois;
	serv->p_get_ip = irc_user_list;
	serv->p_get_ip_uh = irc_userhost;
	serv->p_set_back = irc_set_back;
	serv->p_set_away = irc_set_away;
	serv->p_message = irc_message;
	serv->p_action = irc_action;
	serv->p_notice = irc_notice;
	serv->p_topic = irc_topic;
	serv->p_list_channels = irc_list_channels;
	serv->p_change_nick = irc_change_nick;
	serv->p_names = irc_names;
	serv->p_ping = irc_ping;
	serv->p_raw = irc_raw;
	serv->p_cmp = rfc_casecmp;	/* can be changed by 005 in modes.c */
}

/* hook up signals for IRC protocol implementation. */
void
proto_irc_init(void)
{
	/* server peer messages */
	signal_attach("server peer cap", process_peer_cap);
	signal_attach("server peer invite", process_peer_invite);
	signal_attach("server peer join", process_peer_join);
	signal_attach("server peer kick", process_peer_kick);
	signal_attach("server peer kill", process_peer_kill);
	signal_attach("server peer mode", process_peer_mode);
	signal_attach("server peer nick", process_peer_nick);
	signal_attach("server peer notice", process_peer_notice);
	signal_attach("server peer part", process_peer_part);
	signal_attach("server peer pong", process_peer_pong);
	signal_attach("server peer privmsg", process_peer_privmsg);
	signal_attach("server peer quit", process_peer_quit);
	signal_attach("server peer topic", process_peer_topic);
	signal_attach("server peer wallops", process_peer_wallops);

	signal_attach("server peer", process_named_msg);

	/* server numerics */
	signal_attach("server numeric 001", process_numeric_001);
	signal_attach("server numeric 004", process_numeric_004);
	signal_attach("server numeric 005", process_numeric_005);

	signal_attach("server numeric 263", process_numeric_263);
	signal_attach("server numeric 290", process_numeric_290);

	signal_attach("server numeric 301", process_numeric_301);
	signal_attach("server numeric 302", process_numeric_302);
	signal_attach("server numeric 303", process_numeric_303);
	signal_attach("server numeric 305", process_numeric_305);
	signal_attach("server numeric 306", process_numeric_306);
	signal_attach("server numeric 307", process_whois_identified);
	signal_attach("server numeric 311", process_numeric_311);
	signal_attach("server numeric 311", process_whois_user);
	signal_attach("server numeric 312", process_numeric_312);
	signal_attach("server numeric 313", process_numeric_313);
	signal_attach("server numeric 314", process_whois_user);
	signal_attach("server numeric 315", process_numeric_315);
	signal_attach("server numeric 317", process_numeric_317);
	signal_attach("server numeric 318", process_numeric_318);
	signal_attach("server numeric 319", process_numeric_319);
	signal_attach("server numeric 320", process_whois_identified);
	signal_attach("server numeric 321", process_numeric_321);
	signal_attach("server numeric 322", process_numeric_322);
	signal_attach("server numeric 323", process_numeric_323);
	signal_attach("server numeric 324", process_numeric_324);
	signal_attach("server numeric 329", process_numeric_329);
	signal_attach("server numeric 330", process_numeric_330);
	signal_attach("server numeric 332", process_numeric_332);
	signal_attach("server numeric 333", process_numeric_333);
	signal_attach("server numeric 341", process_numeric_341);
	signal_attach("server numeric 348", process_numeric_348);
	signal_attach("server numeric 349", process_numeric_349);
	signal_attach("server numeric 352", process_numeric_352);
	signal_attach("server numeric 353", process_numeric_353);
	signal_attach("server numeric 354", process_numeric_354);
	signal_attach("server numeric 366", process_numeric_366);
	signal_attach("server numeric 367", process_numeric_367);
	signal_attach("server numeric 368", process_numeric_368);
	signal_attach("server numeric 369", process_whowas_end);
	signal_attach("server numeric 372", process_motd);
	signal_attach("server numeric 375", process_motd);
	signal_attach("server numeric 376", process_motd_end);

	signal_attach("server numeric 406", process_whowas_end);
	signal_attach("server numeric 422", process_motd_end);
	signal_attach("server numeric 432", process_nickname_change_error);
	signal_attach("server numeric 433", process_nickname_change_error);
	signal_attach("server numeric 437", process_numeric_437);
	signal_attach("server numeric 471", process_numeric_471);
	signal_attach("server numeric 473", process_numeric_473);
	signal_attach("server numeric 474", process_numeric_474);
	signal_attach("server numeric 475", process_numeric_475);

	signal_attach("server numeric 600", process_notify_online);
	signal_attach("server numeric 601", process_numeric_601);
	signal_attach("server numeric 604", process_notify_online);
	signal_attach("server numeric 605", process_numeric_605);

	signal_attach("server numeric 730", process_monitor_reply);
	signal_attach("server numeric 731", process_monitor_reply);
	signal_attach("server numeric 732", process_monitor_reply);
	signal_attach("server numeric 733", process_monitor_reply);

	signal_attach("server numeric", process_numeric);

	/* server messages */
	signal_attach("server message error", process_message_error);
	signal_attach("server message notice", process_message_notice);
	signal_attach("server message ping", process_message_ping);

	signal_attach("server message", process_named_servermsg);
}
