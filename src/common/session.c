/* Conspire
 * Copyright (C) 2009 William Pitcock
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
#include "session.h"
#include "format.h"
#include "text.h"

void
session_print_format(session *sess, const gchar *key, ...)
{
	Formatter *f;
	va_list va;
	gchar **data;
	gchar *line;
	gint i;

	g_return_if_fail(sess != NULL);
	g_return_if_fail(key != NULL);

	if ((f = formatter_get(key)) == NULL)
		return;

	data = g_new0(gchar *, f->args + 1);

	va_start(va, key);

	for (i = 0; i < f->args; i++)
		data[i] = va_arg(va, gchar *);

	va_end(va);

	line = formatter_process(f, data);
	if (*line)
		PrintText(sess, line);

	g_free(line);
	g_free(data);
}
