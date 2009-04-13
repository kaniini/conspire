/* Conspire
 * Copyright (C) 2008, 2009 William Pitcock
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

#include "signal_printer.h"
#include "text.h"
#include "modes.h"
#include "xchatc.h"
#include "fe.h"
#include "server.h"

/* DCC */
#include "dcc.h"
#include "network.h"
#include "util.h"

/* actions */

void
signal_printer_action_public(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *text     = params[2];
	gchar *nickchar = params[3];

	session_print_format(sess, "channel action", from, nickchar, text);
}

void
signal_printer_action_public_highlight(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *text     = params[2];
	gchar *nickchar = params[3];

	session_print_format(sess, "channel action hilight", from, nickchar, text);
}

/* Channels */

void
signal_printer_channel_created(gpointer *params)
{
	session *sess    = params[0];
	gchar *channel   = params[1];
	gchar *timestamp = params[2];

	session_print_format(sess, "channel create", channel, timestamp);
}

void
signal_printer_channel_list_head(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "channel list");
}

void
signal_printer_channel_list_entry(gpointer *params)
{
	session *sess = params[0];
	gchar **word = params[1];
	gchar **word_eol = params[2];
	server *serv = sess->server;

	session_print_format(serv->server_session, "channel list entry", word[4], word[5], word_eol[6] + 1);
}

void
signal_printer_channel_modes(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "channel modes", word[4], line[5]);
}

void
signal_printer_channel_join_error(gpointer *params)
{
	session *sess  = params[0];
	gchar *channel = params[1];
	gchar *error   = params[2];

	session_print_format(sess, "channel join error", channel, error);
}

void
signal_printer_channel_invited(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar *nick   = params[2];
	server *serv  = params[3];

	if (word[4][0] == ':')
		session_print_format(sess, "invited", word[4] + 1, nick, serv->servername);
	else
		session_print_format(sess, "invited", word[4], nick, serv->servername);
}

void
signal_printer_channel_users(gpointer *params)
{
	session *sess  = params[0];
	gchar *channel = params[1];
	gchar *nicks   = params[2];

	session_print_format(sess, "users on channel", channel, nicks);
}

void
signal_printer_channel_topic(gpointer *params)
{
	session *sess  = params[0];
	gchar *channel = params[1];
	gchar *topic   = params[2];

	session_print_format(sess, "topic", channel, topic);
}

void
signal_printer_channel_topic_changed(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *topic   = params[2];
	gchar *channel = params[3];

	session_print_format(sess, "topic change", nick, topic, channel);
}
void
signal_printer_channel_topic_date(gpointer *params)
{
	session *sess  = params[0];
	gchar *channel = params[1];
	gchar *nick    = params[2];
	gchar *time    = params[3];

	session_print_format(sess, "topic creation", channel, nick, time);
}

void
signal_printer_channel_join(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *channel = params[2];
	gchar *host    = params[3];

	session_print_format(sess, "join", nick, channel, host);
}

void
signal_printer_channel_kick(gpointer *params)
{
	session *sess  = params[0];
	gchar *kicker  = params[1];
	gchar *nick    = params[2];
	gchar *channel = params[3];
	gchar *reason  = params[4];

	session_print_format(sess, "kick", kicker, nick, channel, reason, 0);
}

void
signal_printer_channel_part(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *host    = params[2];
	gchar *channel = params[3];
	gchar *reason  = params[4];

	if (*reason)
		session_print_format(sess, "part reason", nick, host, channel, reason, 0);
	else
		session_print_format(sess, "part", nick, host, channel);
}

void
signal_printer_channel_quit(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *reason = params[2];
	gchar *host   = params[3];

	session_print_format(sess, "quit", nick, reason, host);
}

/* DCC */

void
signal_printer_dcc_abort(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;

	switch (dcc->type) {
		case TYPE_CHATSEND:
		case TYPE_CHATRECV:
			session_print_format(serv->front_session, "dcc chat abort", dcc->nick);
			break;
		case TYPE_SEND:
			session_print_format(serv->front_session, "dcc send abort", dcc->nick);
			break;
		case TYPE_RECV:
			session_print_format(serv->front_session, "dcc recv abort", dcc->nick);
			break;
	}
}

void
signal_printer_dcc_chat_duplicate(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	session_print_format(sess, "dcc chat reoffer", nick);
}

void
signal_printer_dcc_chat_failed(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *portbuf = params[1];
	gchar *error = params[2];

	session_print_format(serv->front_session, "dcc chat failed", dcc->nick, net_ip(dcc->addr), portbuf, error, 0);
	dcc_close(dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_chat_offer(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	session_print_format(sess, "dcc chat offering", nick);
}

void
signal_printer_dcc_chat_request(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	session_print_format(sess->server->front_session, "dcc chat offer", nick);
}

void
signal_printer_dcc_connected(gpointer *params)
{
	struct DCC *dcc  = params[0];
	server *serv = dcc->serv;
	gchar  *host = params[1];

	switch (dcc->type) {
		case TYPE_SEND:
			session_print_format(serv->front_session, "dcc send connect", dcc->nick, host, dcc->file);
			break;
		case TYPE_RECV:
			session_print_format(serv->front_session, "dcc recv connect", dcc->nick, host, dcc->file);
			break;
		case TYPE_CHATRECV:
			session_print_format(serv->front_session, "dcc chat connect", dcc->nick, host);
			break;
	}
}

void
signal_printer_dcc_failed(gpointer *params)
{
	struct DCC *dcc = params[0];
	gchar *error = params[1];
	server *serv = dcc->serv;
	gchar *type = g_strdup(dcctypes[dcc->type]);

	session_print_format(serv->front_session, "dcc connection failed", type, dcc->nick, error);
	g_free(type);
}

void
signal_printer_dcc_file_complete(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *buf;

	dcc_close (dcc, STAT_DONE, FALSE);
	dcc_calc_average_cps (dcc);	/* this must be done _after_ dcc_close, or dcc_remove_from_sum will see the wrong value in dcc->cps */
	buf = g_strdup_printf("%d", dcc->cps);

	session_print_format(serv->front_session, "dcc recv complete", dcc->file, dcc->destfile, dcc->nick, buf, 0);
	g_free(buf);
}

void
signal_printer_dcc_file_error(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *error = params[1];

	session_print_format(serv->front_session, "dcc recv file open error", dcc->destfile, error);
	dcc_close (dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_file_request(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	gchar *file = params[2];
	gchar *tbuf = params[3];
	session_print_format(sess->server->front_session, "dcc send offer", nick, file, tbuf, tbuf + 24, 0);
}

void
signal_printer_dcc_file_renamed(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *old = params[1];

	session_print_format(serv->front_session, "dcc rename", old, dcc->destfile);
}

void
signal_printer_dcc_file_resume(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	struct DCC *dcc = params[2];
	gchar *tbuf = params[3];

	session_print_format(sess, "dcc resume request", nick, file_part (dcc->file), tbuf);
}

void
signal_printer_dcc_generic_offer(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	gchar *data = params[2];

	session_print_format(sess->server->front_session, "dcc generic offer", data, nick);
}

void
signal_printer_dcc_invalid(gpointer *params)
{
	struct session *sess = params[0];
	session_print_format(sess, "dcc offer not valid");
}

void
signal_printer_dcc_list_start(gpointer *params)
{
	struct session *sess = params[0];

	session_print_format(sess, "dcc header");
}

void
signal_printer_dcc_malformed(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	gchar *data = params[2];
	session_print_format(sess, "dcc malformed", nick, data);
}

void
signal_printer_dcc_recv_error(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *error = params[1];

	session_print_format(serv->front_session, "dcc recv failed", dcc->file, dcc->destfile, dcc->nick, error, 0);
	dcc_close (dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_send_complete(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *buf;

	/* force 100% ack for >4 GB */
	dcc->ack = dcc->size;
	dcc_close (dcc, STAT_DONE, FALSE);
	dcc_calc_average_cps (dcc);

	buf = g_strdup_printf("%d", dcc->cps);
	session_print_format(serv->front_session, "dcc send complete", file_part(dcc->file), dcc->nick, buf);
	g_free(buf);
}

void
signal_printer_dcc_send_failed(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *error = params[1];

	session_print_format(serv->front_session, "dcc send failed", file_part(dcc->file), dcc->nick, error);
	dcc_close (dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_send_request(gpointer *params)
{
	struct session *sess = params[0];
	struct DCC *dcc = params[1];
	gchar *to = params[2];

	session_print_format(sess, "dcc send offer", file_part(dcc->file), to, dcc->file);
}

void
signal_printer_dcc_stoned(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *type = g_strdup(dcctypes[dcc->type]);

	session_print_format(serv->front_session, "dcc timeout", type, file_part(dcc->file), dcc->nick);
	dcc_close(dcc, STAT_ABORTED, FALSE);
}

void
signal_printer_dcc_not_found(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "no dcc");
}


/* non-query private messages */

void
signal_printer_user_message_private(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *message = params[2];

	session_print_format(sess, "message send", nick, message);
}

void
signal_printer_message_private(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *message = params[2];
	gchar *idtext  = params[3];

	if (sess->type == SESS_DIALOG) {
		session_print_format(sess, "private message to dialog", nick, message, idtext);
	} else {
		session_print_format(sess, "private message", nick, message, idtext);
	}
}

void
signal_printer_query_quit(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *reason = params[2];
	gchar *host   = params[3];

	session_print_format(sess, "quit", nick, reason, host);
}

/* channel messages */

void
signal_printer_message_public(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *message  = params[2];
	gchar *nickchar = params[3];
	gchar *idtext   = params[4];

	session_print_format(sess, "channel message", from, nickchar, idtext, message);
}

void
signal_printer_message_public_highlight(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *message  = params[2];
	gchar *nickchar = params[3];
	gchar *idtext   = params[4];

	session_print_format(sess, "channel msg hilight", from, nickchar, idtext, message);
}

/* notices */

void
signal_printer_notice_private(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *message = params[2];

	session_print_format(sess, "notice", nick, message);
}

void
signal_printer_notice_public(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *to      = params[2];
	gchar *message = params[3];

	session_print_format(sess, "channel notice", nick, to, message);
}

/* queries */

void
signal_printer_query_open(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "open dialog");
}

/* server */

void
signal_printer_server_connected(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "connected");
}

void
signal_printer_server_stoned(gpointer *params)
{
	server *serv = params[0];
	gchar *tbuf;

	tbuf = g_strdup_printf("%d", GPOINTER_TO_INT(params[1]));
	session_print_format(serv->server_session, "ping timeout", tbuf);
	serv->auto_reconnect(serv, FALSE, -1);
	g_free(tbuf);
}

void
signal_printer_server_dns_lookup(gpointer *params)
{
	session *sess = params[0];
	gchar *hostname = params[1];

	session_print_format(sess, "server lookup", hostname);
}

void
signal_printer_server_text(gpointer *params)
{
	session *sess  = params[0];
	gchar *text    = params[1];
	gchar *context = params[2];

	session_print_format(sess, "server text", text, context);
}

void
signal_printer_server_motd(gpointer *params)
{
	session *sess = params[0];
	gchar *text   = params[1];

	session_print_format(sess, "motd", text);
}

void
signal_printer_server_wallops(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *text   = params[2];

	session_print_format(sess, "receive wallops", nick, text);
}

void
signal_printer_server_error(gpointer *params)
{
	session *sess = params[0];
	gchar *error  = params[1];

	session_print_format(sess, "server error", error);
}

void
signal_printer_server_notice(gpointer *params)
{
	session *sess = params[0];
	gchar *text   = params[1];

	session_print_format(sess, "server notice", text, sess->server->servername);
}

void
signal_printer_server_kill(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "killed", nick, line[5]);
}

void
signal_printer_server_netsplit(gpointer *params)
{
	session *sess  = params[0];
        server *serv   = params[1];
        gchar *victims = params[2];

	session_print_format(sess, "netsplit", serv->split_serv1, serv->split_serv2, victims);
}

/* whois */

void
signal_printer_whois_server(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *server = params[2];

	session_print_format(sess, "whois server line", nick, server);
}

void
signal_printer_whois_name(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "whois name line", word[4], word[5], word[6], line[8] + 1);
}

void
signal_printer_whois_idle(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *idle   = params[2];

	session_print_format(sess, "whois idle line", nick, idle);
}

void
signal_printer_whois_idle_signon(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];
	gchar *idle   = params[2];
	gchar *signon = params[3];

	session_print_format(sess, "whois idle line with signon", nick, idle, signon);
}

void
signal_printer_whois_end(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];

	session_print_format(sess, "whois end", nick);
}

void
signal_printer_whois_oper(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "whois oper line", word[4], line[5] + 1);
}

void
signal_printer_whois_channels(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "whois channel/oper line", word[4], line[5] + 1);
}

void
signal_printer_whois_identified(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "whois identified", word[4], line[5] + 1);
}

void
signal_printer_whois_authenticated(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "whois authenticated", word[4], line[6] + 1, word[5]);
}

void
signal_printer_whois_generic(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	gchar **line  = params[2];

	session_print_format(sess, "whois special", word[4], (line[5][0] == ':') ? line[5] + 1 : line[5], word[2]);
}

void
signal_printer_whois_away(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *message = params[2];

	session_print_format(sess, "whois away line", nick, message);
}

/* sasl -- temporary */
void
signal_printer_sasl_complete(gpointer *params)
{
	session *sess  = params[0];
	gchar *account = params[1];

	session_print_format(sess, "authenticated to account", account);
}

/* ctcp */
void
signal_printer_ctcp_inbound(gpointer *params)
{
	session *sess = params[0];
	gchar *msg    = params[1];
	gchar *nick   = params[2];
	gchar *to     = params[3];

	if(!is_channel(sess->server, to))
	{
		session_print_format(sess->server->front_session, "ctcp generic", msg, nick);
	}
	else
	{
		session *chansess = find_channel(sess->server, to);
		if (!chansess)
			chansess = sess;

		session_print_format(chansess, "ctcp generic to channel", msg, nick, to);
	}
}

/* user-sent signals */

void
signal_printer_user_invite(gpointer *params)
{
	session *sess = params[0];
	gchar **word  = params[1];
	server *serv  = params[2];

	session_print_format(sess, "your invitation", word[4], word[5], serv->servername);
}

void
signal_printer_user_message_public(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *message  = params[2];
	gchar *nickchar = params[3];

	session_print_format(sess, "your message", from, message, nickchar);
}

void
signal_printer_user_action(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *text     = params[2];
	gchar *nickchar = params[3];

	session_print_format(sess, "your action", from, text, nickchar);
}

void
signal_printer_user_nick_changed(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *newnick = params[2];

	session_print_format(sess, "your nick changing", nick, newnick);
}

void
signal_printer_user_joined(gpointer *params)
{
	session *sess  = params[0];
        gchar *nick    = params[1];
        gchar *channel = params[2];
        gchar *host    = params[3];

	session_print_format(sess, "you join", nick, channel, host);
}

void
signal_printer_user_kicked(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *channel = params[2];
	gchar *kicker  = params[3];
	gchar *reason  = params[4];

	session_print_format(sess, "you kicked", nick, channel, kicker, reason);
}

void
signal_printer_user_part(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *host    = params[2];
	gchar *channel = params[3];
	gchar *reason  = params[4];

	if (*reason)
		session_print_format(sess, "you part with reason", nick, host, channel, reason);
	else
		session_print_format(sess, "you part", nick, host, channel);
}

/* Plugins */
void
signal_printer_plugin_loaded(gpointer *params)
{
	gchar *name    = params[0];
	gchar *version = params[1];

	fe_pluginlist_update();

	session_print_format(current_sess, "plugin loaded", name, version);
}

void
signal_printer_plugin_unloaded(gpointer *params)
{
	gchar *name    = params[0];

	fe_pluginlist_update();

	session_print_format(current_sess, "plugin unloaded", name);
}

void
signal_printer_plugin_error(gpointer *params)
{
	gchar *name    = params[0];
	gchar *message = params[1];

	session_print_format(current_sess, "plugin error", name, message);
}

void
signal_printer_nick_changed(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *newnick = params[2];

	session_print_format(sess, "change nick", nick, newnick);
}

void
signal_printer_nick_clash(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *newnick = params[2];

	session_print_format(sess, "nick clash", nick, newnick);
}

void
signal_printer_nick_error(gpointer *params)
{
	session *sess  = params[0];

	session_print_format(sess, "nick failed");
}

void
signal_printer_server_ping_reply(gpointer *params)
{
	session *sess  = params[0];
	gchar *from    = params[1];
	gchar *content = params[2];

	session_print_format(sess, "ping reply", from, content);
}

void
signal_printer_server_numeric_302(gpointer *params)
{
	session *sess   = params[0];
        gchar *hostname = params[3];

	session_print_format(sess, "found ip", hostname);
}

void
signal_printer_server_unknown(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "unknown host");
}

void
signal_printer_server_connect(gpointer *params)
{
	session *sess = params[0];
	gchar *host   = params[1];
	gchar *ip     = params[2];
	gchar *data   = params[3];

	session_print_format(sess, "connecting", host, ip, data);
}

void
signal_printer_server_connect_halted(gpointer *params)
{
	session *sess = params[0];
	gchar *data   = params[1];

	session_print_format(sess, "stop connection", data);
}

void
signal_printer_server_disconnected(gpointer *params)
{
	session *sess = params[0];
	gchar *error  = params[1];

	session_print_format(sess, "disconnected", error);
}

void
signal_printer_ctcp_reply(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	gchar *type    = params[2];
        gchar *content = params[3];

	session_print_format(sess, "ctcp reply generic", nick, type, content);
}

void
signal_printer_ctcp_send(gpointer *params)
{
	session *sess  = params[0];
	gchar *target  = params[1];
	gchar *message = params[2];

	session_print_format(sess, "ctcp send", target, message);
}

void
signal_printer_channel_bans(gpointer *params)
{
	session *sess   = params[0];
        gchar *channel  = params[1];
        gchar *mask     = params[2];
        gchar *nick     = params[3];
        gchar *time_set = params[4];

	session_print_format(sess, "ban list", channel, mask, nick, time_set);
}

void
signal_printer_channel_modes_raw(gpointer *params)
{
	session *sess = params[0];
        gchar *nick   = params[1];
        gchar *modes  = params[2];

	session_print_format(sess, "raw modes", nick, modes);
}

void
signal_printer_exec_already_running(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "process already running");
}

void
signal_printer_ignore_added(gpointer *params)
{
	session *sess = params[0];
        gchar **word  = params[1];

	session_print_format(sess, "ignore add", word[2]);
}

void
signal_printer_ignore_changed(gpointer *params)
{
	session *sess = params[0];
        gchar **word  = params[1];

	session_print_format(sess, "ignore changed", word[2]);
}

void
signal_printer_ignore_list_empty(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "ignorelist empty");
}


void
signal_printer_ignore_list_footer(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "ignorelist footer");
}

void
signal_printer_ignore_list_header(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "ignorelist header");
}

void
signal_printer_ignore_removed(gpointer *params)
{
	session *sess = params[0];
	gchar *mask   = params[1];

	session_print_format(sess, "ignore remove", mask);
}

void
signal_printer_user_notice(gpointer *params)
{
	session *sess    = params[0];
	gchar **word     = params[1];
	gchar **word_eol = params[2];

	session_print_format(sess, "notice send", word[2], word_eol[3]);
}

/* /notify stuff */
void
signal_printer_notify_removed(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];

	session_print_format(sess, "delete notify", nick);
}

void
signal_printer_notify_added(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];

	session_print_format(sess, "add notify", nick);
}

void
signal_printer_notify_list_header(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "notify header");
}

void
signal_printer_notify_list_total(gpointer *params)
{
	session *sess = params[0];
	gchar *total  = params[1];

	session_print_format(sess, "notify number", total);
}

void
signal_printer_notify_list_empty(gpointer *params)
{
	session *sess = params[0];

	session_print_format(sess, "notify empty");
}

void
signal_printer_notify_offline(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	server *serv   = params[2];

	gchar *servername = g_strdup(serv->servername);
	gchar *network    = g_strdup(server_get_network(serv, TRUE));

	session_print_format(sess, "notify offline", nick, servername, network);

	g_free(servername);
	g_free(network);
}

void
signal_printer_notify_online(gpointer *params)
{
	session *sess  = params[0];
	gchar *nick    = params[1];
	server *serv   = params[2];

	gchar *servername = g_strdup((gchar *)serv->servername);
	gchar *network    = g_strdup((gchar *)server_get_network(serv, TRUE));

	session_print_format(sess, "notify online", nick, servername, network);

	g_free(servername);
	g_free(network);
}

void
signal_printer_init(void)
{
	/* Actions */
	signal_attach("action public",          signal_printer_action_public);
	signal_attach("action public hilight",  signal_printer_action_public_highlight);

	/* Channel events */
	signal_attach("channel bans",           signal_printer_channel_bans);
	signal_attach("channel created",        signal_printer_channel_created);
	signal_attach("channel invited",        signal_printer_channel_invited);
	signal_attach("channel join",           signal_printer_channel_join);
	signal_attach("channel join error",     signal_printer_channel_join_error);
	signal_attach("channel kick",           signal_printer_channel_kick);
	signal_attach("channel list entry",     signal_printer_channel_list_entry);
	signal_attach("channel list head",      signal_printer_channel_list_head);
	signal_attach("channel modes",          signal_printer_channel_modes);
	signal_attach("channel modes raw",      signal_printer_channel_modes_raw);
	signal_attach("channel part",           signal_printer_channel_part);
	signal_attach("channel quit",           signal_printer_channel_quit);
	signal_attach("channel topic",          signal_printer_channel_topic);
	signal_attach("channel topic changed",  signal_printer_channel_topic_changed);
	signal_attach("channel topic date",     signal_printer_channel_topic_date);
	signal_attach("channel users",          signal_printer_channel_users);

	/* CTCPs */
	signal_attach("ctcp inbound",           signal_printer_ctcp_inbound);
	signal_attach("ctcp reply",             signal_printer_ctcp_reply);
	signal_attach("ctcp send",              signal_printer_ctcp_send);

	/* DCCs */
	signal_attach("dcc abort",              signal_printer_dcc_abort);
	signal_attach("dcc chat duplicate",     signal_printer_dcc_chat_duplicate);
	signal_attach("dcc chat failed",        signal_printer_dcc_chat_failed);
	signal_attach("dcc chat offer",         signal_printer_dcc_chat_offer);
	signal_attach("dcc chat request",       signal_printer_dcc_chat_request);
	signal_attach("dcc connected",          signal_printer_dcc_connected);
	signal_attach("dcc failed",             signal_printer_dcc_failed);
	signal_attach("dcc file complete",      signal_printer_dcc_file_complete);
	signal_attach("dcc file error",         signal_printer_dcc_file_error);
	signal_attach("dcc file renamed",       signal_printer_dcc_file_renamed);
	signal_attach("dcc file request",       signal_printer_dcc_file_request);
	signal_attach("dcc file resume",        signal_printer_dcc_file_resume);
	signal_attach("dcc generic offer",      signal_printer_dcc_generic_offer);
	signal_attach("dcc invalid",            signal_printer_dcc_invalid);
	signal_attach("dcc list start",         signal_printer_dcc_list_start);
	signal_attach("dcc malformed",          signal_printer_dcc_malformed);
	signal_attach("dcc not found",          signal_printer_dcc_not_found);
	signal_attach("dcc recv error",         signal_printer_dcc_recv_error);
	signal_attach("dcc send complete",      signal_printer_dcc_send_complete);
	signal_attach("dcc send failed",        signal_printer_dcc_send_failed);
	signal_attach("dcc send request",       signal_printer_dcc_send_request);
	signal_attach("dcc stoned",             signal_printer_dcc_stoned);

	/* Exec - XXX - this needs to be moved into the exec plugin */
	signal_attach("exec already running",   signal_printer_exec_already_running);

	/* Ignore list */
	signal_attach("ignore added",           signal_printer_ignore_added);
	signal_attach("ignore changed",         signal_printer_ignore_changed);
	signal_attach("ignore list empty",      signal_printer_ignore_list_empty);
	signal_attach("ignore list footer",     signal_printer_ignore_list_footer);
	signal_attach("ignore list header",     signal_printer_ignore_list_header);
	signal_attach("ignore removed",         signal_printer_ignore_removed);

	/* Messages */
	signal_attach("message private",        signal_printer_message_private);
	signal_attach("message public",         signal_printer_message_public);
	signal_attach("message public hilight", signal_printer_message_public_highlight);

	/* Nicks */
	signal_attach("nick changed",           signal_printer_nick_changed);
	signal_attach("nick clash",             signal_printer_nick_clash);
	signal_attach("nick error",             signal_printer_nick_error);

	/* Notices */
	signal_attach("notice private",         signal_printer_notice_private);
	signal_attach("notice public",          signal_printer_notice_public);

	/* Notify list */
	signal_attach("notify added",           signal_printer_notify_added);
	signal_attach("notify list empty",      signal_printer_notify_list_empty);
	signal_attach("notify list header",     signal_printer_notify_list_header);
	signal_attach("notify list total",      signal_printer_notify_list_total);
	signal_attach("notify offline",         signal_printer_notify_offline);
	signal_attach("notify online",          signal_printer_notify_online);
	signal_attach("notify removed",         signal_printer_notify_removed);

	/* Plugins */
	signal_attach("plugin error",           signal_printer_plugin_error);
	signal_attach("plugin loaded",          signal_printer_plugin_loaded);
	signal_attach("plugin unloaded",        signal_printer_plugin_unloaded);

	/* Queries */
	signal_attach("query open",             signal_printer_query_open);
	signal_attach("query quit",             signal_printer_query_quit);

	/* SASL */
	signal_attach("sasl complete",          signal_printer_sasl_complete);

	/* Server events */
	signal_attach("server connect",         signal_printer_server_connect);
	signal_attach("server connect halted",  signal_printer_server_connect_halted);
	signal_attach("server connected",       signal_printer_server_connected);
	signal_attach("server disconnected",    signal_printer_server_disconnected);
	signal_attach("server dns lookup",      signal_printer_server_dns_lookup);
	signal_attach("server error",           signal_printer_server_error);
	signal_attach("server kill",            signal_printer_server_kill);
	signal_attach("server motd",            signal_printer_server_motd);
	signal_attach("server netsplit",        signal_printer_server_netsplit);
	signal_attach("server notice",          signal_printer_server_notice);
	signal_attach("server numeric 302",     signal_printer_server_numeric_302);
	signal_attach("server ping reply",      signal_printer_server_ping_reply);
	signal_attach("server stoned",          signal_printer_server_stoned);
	signal_attach("server text",            signal_printer_server_text);
	signal_attach("server unknown",         signal_printer_server_unknown);
	signal_attach("server wallops",         signal_printer_server_wallops);

	/* User-generated or related events */
	signal_attach("user action",            signal_printer_user_action);
	signal_attach("user invite",            signal_printer_user_invite);
	signal_attach("user joined",            signal_printer_user_joined);
	signal_attach("user kicked",            signal_printer_user_kicked);
	signal_attach("user message private",   signal_printer_user_message_private);
	signal_attach("user message public",    signal_printer_user_message_public);
	signal_attach("user nick changed",      signal_printer_user_nick_changed);
	signal_attach("user notice",            signal_printer_user_notice);
	signal_attach("user part",              signal_printer_user_part);

	/* Whois */
	signal_attach("whois authenticated",    signal_printer_whois_authenticated);
	signal_attach("whois away",             signal_printer_whois_away);
	signal_attach("whois channels",         signal_printer_whois_channels);
	signal_attach("whois end",              signal_printer_whois_end);
	signal_attach("whois generic",          signal_printer_whois_generic);
	signal_attach("whois identified",       signal_printer_whois_identified);
	signal_attach("whois idle",             signal_printer_whois_idle);
	signal_attach("whois idle signon",      signal_printer_whois_idle_signon);
	signal_attach("whois name",             signal_printer_whois_name);
	signal_attach("whois oper",             signal_printer_whois_oper);
	signal_attach("whois server",           signal_printer_whois_server);
}

