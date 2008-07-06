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
#include <mowgli.h>

#include "linequeue.h"

/*
 * TODO:
 *     - combine multiple lines into a single write
 */

/* list of queues to add additional write tokens to */
static GList *queues = NULL;
static gint lqat_tag = 0;

/* replinishes the amount of write operations allowed. */
gboolean
linequeue_add_tokens(gpointer unused)
{
	GList *iter;

	MOWGLI_ITER_FOREACH(iter, queues)
	{
		LineQueue *lq = iter->data;

		if (lq->writeoffs == 0)
			continue;

		lq->writeoffs--;

		if (!g_queue_is_empty(&lq->q))
			linequeue_flush(lq);
	}

	return TRUE;
}

LineQueue *
linequeue_new(gpointer data, LineQueueWriter w)
{
	LineQueue *lq = g_slice_new0(LineQueue);

	lq->data = data;
	lq->w = w;
	lq->available = 5;	/* XXX: making this a config option seems like a good idea. */

	queues = g_list_prepend(queues, lq);

	if (!lqat_tag)
		lqat_tag = g_timeout_add(250, linequeue_add_tokens, NULL);

	return lq;
}

void
linequeue_add_line(LineQueue *lq, gchar *line)
{
	g_return_if_fail(lq != NULL);

	g_queue_push_tail(&lq->q, g_strdup(line));
}

void
linequeue_flush(LineQueue *lq)
{
	gchar *line;

	g_return_if_fail(lq != NULL);

	while ((line = g_queue_pop_head(&lq->q)) != NULL)
	{
		lq->w(lq->data, line, strlen(line));
		g_free(line);

		lq->writeoffs++;
		if (lq->writeoffs >= lq->available)
			break;
	}
}

void
linequeue_erase(LineQueue *lq)
{
	gchar *line;

	g_return_if_fail(lq != NULL);

	while ((line = g_queue_pop_head(&lq->q)) != NULL)
		g_free(line);
}

void
linequeue_destroy(LineQueue *lq)
{
	g_return_if_fail(lq != NULL);

	linequeue_erase(lq);
	g_slice_free(LineQueue, lq);
}
