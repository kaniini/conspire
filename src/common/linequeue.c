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

#include "linequeue.h"

/*
 * TODO:
 *     - combine multiple lines into a single write
 *     - write scheduling.
 */

LineQueue *
linequeue_new(gpointer data, LineQueueWriter w)
{
	LineQueue *lq = g_slice_new0(LineQueue);

	lq->data = data;
	lq->w = w;

	return lq;
}

void
linequeue_add_line(LineQueue *lq, gchar *line)
{
	g_return_if_fail(lq != NULL);

	g_queue_push_tail(lq->q, g_strdup(line));
}

void
linequeue_flush(LineQueue *lq)
{
	gchar *line;

	g_return_if_fail(lq != NULL);

	while ((line = g_queue_pop_head(lq->q)) != NULL)
	{
		lq->w(lq->data, line, strlen(line));
		g_free(line);
	}
}
