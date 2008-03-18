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

#include "signal_printer.h"
#include "text.h"

/* DCC */
#include "dcc.h"
#include "network.h"
#include "util.h"

void
signal_printer_dcc_abort(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;

	switch (dcc->type) {
		case TYPE_CHATSEND:
		case TYPE_CHATRECV:
			EMIT_SIGNAL (XP_TE_DCCCHATABORT, serv->front_session, dcc->nick, NULL, NULL, NULL, 0);
			break;
		case TYPE_SEND:
			EMIT_SIGNAL (XP_TE_DCCSENDABORT, serv->front_session, dcc->nick, NULL, NULL, NULL, 0);
			break;
		case TYPE_RECV:
			EMIT_SIGNAL (XP_TE_DCCRECVABORT, serv->front_session, dcc->nick, NULL, NULL, NULL, 0);
			break;
	}
}

void
signal_printer_dcc_chat_duplicate(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	EMIT_SIGNAL (XP_TE_DCCCHATREOFFER, sess, nick, NULL, NULL, NULL, 0);
}

void
signal_printer_dcc_chat_failed(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *portbuf = params[1];
	gchar *error = params[2];

	EMIT_SIGNAL (XP_TE_DCCCHATF, serv->front_session, dcc->nick, net_ip(dcc->addr), portbuf, error, 0);
	dcc_close(dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_chat_offer(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	EMIT_SIGNAL (XP_TE_DCCCHATOFFERING, sess, nick, NULL, NULL, NULL, 0);
}

void
signal_printer_dcc_chat_request(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	EMIT_SIGNAL (XP_TE_DCCCHATOFFER, sess->server->front_session, nick, NULL, NULL, NULL, 0);
}

void
signal_printer_dcc_connected(gpointer *params)
{
	struct DCC *dcc  = params[0];
	server *serv = dcc->serv;
	gchar  *host = params[1];

	switch (dcc->type) {
		case TYPE_SEND:
			EMIT_SIGNAL (XP_TE_DCCCONSEND, serv->front_session, dcc->nick, host, dcc->file, NULL, 0);
			break;
		case TYPE_RECV:
			EMIT_SIGNAL (XP_TE_DCCCONRECV, serv->front_session, dcc->nick, host, dcc->file, NULL, 0);
			break;
		case TYPE_CHATRECV:
			EMIT_SIGNAL (XP_TE_DCCCONCHAT, serv->front_session, dcc->nick, host, NULL, NULL, 0);
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

	EMIT_SIGNAL (XP_TE_DCCCONFAIL, serv->front_session, type, dcc->nick, error, NULL, 0);
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

	EMIT_SIGNAL (XP_TE_DCCRECVCOMP, serv->front_session, dcc->file, dcc->destfile, dcc->nick, buf, 0);
	g_free(buf);
}

void
signal_printer_dcc_file_error(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *error = params[1];

	EMIT_SIGNAL (XP_TE_DCCFILEERR, serv->front_session, dcc->destfile, error, NULL, NULL, 0);
	dcc_close (dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_file_request(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	gchar *file = params[2];
	gchar *tbuf = params[3];
	EMIT_SIGNAL (XP_TE_DCCSENDOFFER, sess->server->front_session, nick, file, tbuf, tbuf + 24, 0);
}

void
signal_printer_dcc_file_renamed(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *old = params[1];

	EMIT_SIGNAL (XP_TE_DCCRENAME, serv->front_session, old, dcc->destfile, NULL, NULL, 0);
}

void
signal_printer_dcc_file_resume(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	struct DCC *dcc = params[2];
	gchar *tbuf = params[3];

	EMIT_SIGNAL (XP_TE_DCCRESUMEREQUEST, sess, nick, file_part (dcc->file), tbuf, NULL, 0);
}

void
signal_printer_dcc_generic_offer(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	gchar *data = params[2];

	EMIT_SIGNAL (XP_TE_DCCGENERICOFFER, sess->server->front_session, data, nick, NULL, NULL, 0);
}

void
signal_printer_dcc_invalid(gpointer *params)
{
	struct session *sess = params[0];
	EMIT_SIGNAL (XP_TE_DCCIVAL, sess, NULL, NULL, NULL, NULL, 0);
}

void
signal_printer_dcc_list_start(gpointer *params)
{
	struct session *sess = params[0];

	EMIT_SIGNAL (XP_TE_DCCHEAD, sess, NULL, NULL, NULL, NULL, 0);
}

void
signal_printer_dcc_malformed(gpointer *params)
{
	struct session *sess = params[0];
	gchar *nick = params[1];
	gchar *data = params[2];
	EMIT_SIGNAL (XP_TE_MALFORMED, sess, nick, data, NULL, NULL, 0);
}

void
signal_printer_dcc_recv_error(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *error = params[1];

	EMIT_SIGNAL (XP_TE_DCCRECVERR, serv->front_session, dcc->file, dcc->destfile, dcc->nick, error, 0);
	dcc_close (dcc, STAT_FAILED, FALSE);
}

void
signal_printer_dcc_stoned(gpointer *params)
{
	struct DCC *dcc = params[0];
	server *serv = dcc->serv;
	gchar *type = g_strdup(dcctypes[dcc->type]);

	EMIT_SIGNAL (XP_TE_DCCTOUT, serv->front_session, type, file_part(dcc->file), dcc->nick, NULL, 0);
	dcc_close(dcc, STAT_ABORTED, FALSE); 
}

/* server */

void
signal_printer_server_connected(gpointer *params)
{
	server *serv = params[0];

	EMIT_SIGNAL (XP_TE_CONNECTED, serv->server_session, NULL, NULL, NULL, NULL, 0);
}

void
signal_printer_server_stoned(gpointer *params)
{
	server *serv = params[0];
	gchar *tbuf;

	tbuf = g_strdup_printf("%d", GPOINTER_TO_INT(params[1]));
	EMIT_SIGNAL(XP_TE_PINGTIMEOUT, serv->server_session, tbuf, NULL, NULL, NULL, 0);
	serv->auto_reconnect(serv, FALSE, -1);
	g_free(tbuf);
}

void
signal_printer_server_dns_lookup(gpointer *params)
{
	session *sess = params[0];
	gchar *hostname = params[1];

	EMIT_SIGNAL (XP_TE_SERVERLOOKUP, sess, hostname, NULL, NULL, NULL, 0);
}

void
signal_printer_init(void)
{
	/* DCC */
	signal_attach("dcc abort",          signal_printer_dcc_abort);
	signal_attach("dcc chat duplicate", signal_printer_dcc_chat_duplicate);
	signal_attach("dcc chat failed",    signal_printer_dcc_chat_failed);
	signal_attach("dcc chat offer",     signal_printer_dcc_chat_offer);
	signal_attach("dcc chat request",   signal_printer_dcc_chat_request);
	signal_attach("dcc connected",      signal_printer_dcc_connected);
	signal_attach("dcc failed",         signal_printer_dcc_failed);
	signal_attach("dcc file complete",  signal_printer_dcc_file_complete);
	signal_attach("dcc file error",     signal_printer_dcc_file_error);
	signal_attach("dcc file request",   signal_printer_dcc_file_request);
	signal_attach("dcc file renamed",   signal_printer_dcc_file_renamed);
	signal_attach("dcc file resume",    signal_printer_dcc_file_resume);
	/* if anyone can figure out why this would be necessary, I'd love to know. */
	signal_attach("dcc generic offer",  signal_printer_dcc_generic_offer);
	signal_attach("dcc invalid",        signal_printer_dcc_invalid);
	signal_attach("dcc list start",     signal_printer_dcc_list_start);
	signal_attach("dcc malformed",      signal_printer_dcc_malformed);
	signal_attach("dcc recv error",     signal_printer_dcc_recv_error);
	signal_attach("dcc stoned",         signal_printer_dcc_stoned);

	/* server */
	signal_attach("server connected",   signal_printer_server_connected);
	signal_attach("server dns lookup",  signal_printer_server_dns_lookup);
	signal_attach("server stoned",      signal_printer_server_stoned);
}
