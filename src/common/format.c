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

#include <mowgli.h>
#include <glib.h>

#include "format.h"
#include "signal_factory.h"
#include "xchatc.h"

static int formatter_initialized = 0;
mowgli_dictionary_t *formatters = NULL;

Formatter *
formatter_register(const gchar *key, const gchar *format, int args)
{
	Formatter *f;

	if (!formatter_initialized)
	{
		formatters = mowgli_dictionary_create(g_ascii_strcasecmp);
		formatter_initialized++;
	}

	f = g_slice_new0(Formatter);
	f->key = g_strdup(key);
	f->format = g_strdup(format);
	f->args = args;

	mowgli_dictionary_add(formatters, key, f);

	return f;
}

Formatter *
formatter_get(const gchar *key)
{
	return mowgli_dictionary_retrieve(formatters, key);
}

static gchar *
formatter_replace(gchar *s, int size, const gchar *old, const gchar *new)
{
	gchar *ptr = s;
	gint left, avail, oldlen, newlen, diff;

	if (old == NULL || new == NULL)
		return s;

	left = strlen(s);
	avail = size - (left + 1);
	oldlen = strlen(old);
	newlen = strlen(new);
	diff = newlen - oldlen;

	while (left >= oldlen)
	{
		if (strncmp(ptr, old, oldlen))
		{
			left--;
			ptr++;
			continue;
		}

		if (diff > avail)
			break;

		if (diff != 0)
			memmove(ptr + oldlen + diff, ptr + oldlen, left + 1 - oldlen);

		memcpy(ptr, new, newlen);
		ptr += newlen;
		left -= oldlen;
	}

	return s;
}

gchar *
formatter_process(Formatter *f, gchar **data)
{
	gint i;
	gchar buf[4096];
	gchar *signame;

	g_return_val_if_fail(f != NULL, NULL);
	g_return_val_if_fail(data != NULL, NULL);

	g_strlcpy(buf, f->format, 4096);

	signame = g_strdup_printf("format %s", f->key);
	signal_emit(signame, 2, f->args, data);
	g_free(signame);

	for (i = 1; i <= f->args; i++)
	{
		gchar *token = g_strdup_printf("$%d", i);

		formatter_replace(buf, 4096, token, data[i - 1]);

		g_free(token);
	}

	formatter_replace(buf, 4096, "%B", "\x02");
	formatter_replace(buf, 4096, "%C", "\x03");
	formatter_replace(buf, 4096, "%O", "\x0F");
	formatter_replace(buf, 4096, "%U", "\x1F");
	formatter_replace(buf, 4096, "$t", prefs.indent_nicks ? "\t" : " ");
	formatter_replace(buf, 4096, "%H", prefs.indent_nicks ? "\x08" : " ");	/* XXX: maybe we should always do this? */

	return g_strdup(buf);
}

void
formatter_remove(const gchar *key)
{
	Formatter *f;

	f = mowgli_dictionary_retrieve(formatters, key);
	if (f == NULL)
		return;

	mowgli_dictionary_delete(formatters, key);

	g_free(f->key);
	g_free(f->format);

	g_slice_free(Formatter, f);
}
