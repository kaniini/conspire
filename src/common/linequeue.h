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

#include <glib.h>

#ifndef __CONSPIRE_LINEQUEUE_H__GUARD
#define __CONSPIRE_LINEQUEUE_H__GUARD

typedef int (*LineQueueWriter)(gpointer data, gchar *line, gint len);
typedef int (*LineQueueUpdater)(gpointer data);

typedef struct {
    GQueue q;
    gpointer data;
    LineQueueWriter w;
    LineQueueUpdater update;
    gint available;
    gint writeoffs;
} LineQueue;

LineQueue *linequeue_new(gpointer data, LineQueueWriter w, LineQueueUpdater u);
void linequeue_add_line(LineQueue *lq, gchar *line);
void linequeue_flush(LineQueue *lq);
void linequeue_destroy(LineQueue *lq);
void linequeue_erase(LineQueue *lq);

static inline gint
linequeue_size(LineQueue *lq)
{
	g_return_val_if_fail(lq != NULL, -1);

	return (gint) g_queue_get_length(&lq->q);
}

#endif
