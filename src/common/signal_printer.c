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
signal_printer_init(void)
{
	signal_attach("server stoned", signal_printer_server_stoned);
}
