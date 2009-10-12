/* Conspire
 * Copyright © 2009 William Pitcock <nenolod@dereferenced.org>.
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
#include <fcntl.h>
#include "../common/stdinc.h"
#include <stdlib.h>

#include "fe-gtk.h"

#include <gtk/gtkbutton.h>
#include <gtk/gtkhbbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkvscrollbar.h>
#include <gtk/gtkstock.h>

#include "../common/xchat.h"
#include "../common/fe.h"
#include "../common/xchatc.h"
#include "../common/cfgfiles.h"
#include "../common/server.h"
#include "gtkutil.h"
#include "palette.h"
#include "maingui.h"
#include "rawlog.h"
#include "textgui.h"
#include "xtext.h"
#include "conversation-window.h"

/*
 * Old xtext API.
 */

#ifndef CONVERSATION_WINDOW_USE_WEBKIT

typedef struct {
	ConversationWindow public_info;
	GtkWidget *xtext, *vs;
} ConversationWindowPriv;

ConversationWindow *
conversation_window_new(void)
{
	ConversationWindowPriv *priv_win = g_slice_new0(ConversationWindowPriv);
	ConversationWindow *win = (ConversationWindow *) priv_win;

	win->widget = gtk_hbox_new(FALSE, 2);

	priv_win->xtext = gtk_xtext_new(colors, 0);
	gtk_box_pack_start(GTK_BOX(win->widget), priv_win->xtext, TRUE, TRUE, 0);

	priv_win->vs = gtk_vscrollbar_new(GTK_XTEXT(priv_win->xtext)->adj);
	gtk_box_pack_start(GTK_BOX(win->widget), priv_win->vs, FALSE, FALSE, 0);

	gtk_widget_show_all(win->widget);

	return win;
}

void
conversation_window_update_preferences(ConversationWindow *win)
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;

	g_return_if_fail(win != NULL);
	g_return_if_fail(priv_win->xtext != NULL);

	gtk_xtext_set_palette(GTK_XTEXT(priv_win->xtext), colors);
	gtk_xtext_set_max_indent(GTK_XTEXT(priv_win->xtext), prefs.max_auto_indent);
	gtk_xtext_set_thin_separator(GTK_XTEXT(priv_win->xtext), prefs.thin_separator);
	gtk_xtext_set_max_lines(GTK_XTEXT(priv_win->xtext), prefs.max_lines);
	gtk_xtext_set_wordwrap(GTK_XTEXT(priv_win->xtext), prefs.wordwrap);
	gtk_xtext_set_show_marker(GTK_XTEXT(priv_win->xtext), prefs.show_marker);
	gtk_xtext_set_show_separator(GTK_XTEXT(priv_win->xtext), prefs.show_separator);
	gtk_xtext_set_indent(GTK_XTEXT(priv_win->xtext), prefs.indent_nicks);

	if (!gtk_xtext_set_font(GTK_XTEXT(priv_win->xtext), prefs.font_normal))
	{
		fe_message ("No font is available", FE_MSG_WAIT | FE_MSG_ERROR);
		exit(1);
	}

	gtk_xtext_refresh(GTK_XTEXT(priv_win->xtext));
}

void
conversation_window_set_urlcheck_function(ConversationWindow *win, int (*urlcheck_function) (GtkWidget *, char *, int))
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;
	
	g_return_if_fail(win != NULL);
	g_return_if_fail(priv_win->xtext != NULL);

	gtk_xtext_set_urlcheck_function(GTK_XTEXT(priv_win->xtext), urlcheck_function);
}

gpointer
conversation_window_get_opaque_buffer(ConversationWindow *win)
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;

	g_return_val_if_fail(win != NULL, NULL);
	g_return_val_if_fail(priv_win->xtext != NULL, NULL);

	return (GTK_XTEXT(priv_win->xtext)->buffer);
}

void
conversation_window_set_opaque_buffer(ConversationWindow *win, gpointer buf)
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;

	g_return_if_fail(win != NULL);
	g_return_if_fail(priv_win->xtext != NULL);

	gtk_xtext_buffer_show(GTK_XTEXT(priv_win->xtext), buf, TRUE);
}

gpointer
conversation_buffer_new(ConversationWindow *win, gboolean timestamp)
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;
	gpointer buf;

	g_return_val_if_fail(win != NULL, NULL);
	g_return_val_if_fail(priv_win->xtext != NULL, NULL);

	buf = gtk_xtext_buffer_new(GTK_XTEXT(priv_win->xtext));
	conversation_buffer_set_time_stamp(buf, timestamp);

	return buf;
}

void
conversation_buffer_set_time_stamp(gpointer buf, gboolean timestamp)
{
	g_return_if_fail(buf != NULL);

	gtk_xtext_set_time_stamp(buf, timestamp);
	((xtext_buffer *)buf)->needs_recalc = TRUE;
}

void
conversation_buffer_append_text(gpointer buf, guchar *text, time_t stamp)
{
	g_return_if_fail(buf != NULL);
	g_return_if_fail(text != NULL);

	PrintTextRaw(buf, text, prefs.indent_nicks, stamp);
}

void
conversation_buffer_clear(gpointer buf)
{
	g_return_if_fail(buf != NULL);

	gtk_xtext_clear(buf);
}

void
conversation_window_append_text(ConversationWindow *win, guchar *text, time_t stamp)
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;

	g_return_if_fail(win != NULL);
	g_return_if_fail(priv_win->xtext != NULL);
	g_return_if_fail(text != NULL);

	conversation_buffer_append_text(GTK_XTEXT(priv_win->xtext)->buffer, text, stamp);
}

void
conversation_window_clear(ConversationWindow *win)
{
	ConversationWindowPriv *priv_win = (ConversationWindowPriv *) win;

	g_return_if_fail(win != NULL);
	g_return_if_fail(priv_win->xtext != NULL);

	conversation_buffer_clear(GTK_XTEXT(priv_win->xtext)->buffer);
}

#else



#endif
