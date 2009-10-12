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

#ifndef __CONSPIRE__CONVERSATION_WINDOW__H__GUARD
#define __CONSPIRE__CONVERSATION_WINDOW__H__GUARD

/*
 * Public structure for the conversation window.
 * Contains a pointer to a widget, and the engine ID as string.
 */
typedef struct {
    GtkWidget *widget;
    const gchar *engineid;
} ConversationWindow;

#define CONVERSATION_WIDGET(win) ((ConversationWindow *)win->widget)

ConversationWindow *conversation_window_new(void);

gpointer conversation_window_get_opaque_buffer(ConversationWindow *win);
void conversation_window_set_opaque_buffer(ConversationWindow *win, gpointer buf);

void conversation_buffer_append_text(gpointer buf, guchar *text, time_t stamp);
void conversation_buffer_clear(gpointer buf);

void conversation_window_append_text(ConversationWindow *win, guchar *text, time_t stamp);
void conversation_window_clear(ConversationWindow *win);

#endif
