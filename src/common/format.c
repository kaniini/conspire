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

/**
 * formatter_process_real:
 * 	Replaces control codes in a string.  The string must be zero-terminated.
 *
 * Inputs:
 *	src - the source format string (e.g. a formatter::format).
 *	buf - destination buffer
 *
 * Outputs:
 *      pointer to buf or NULL on failure.
 *
 * Side Effects:
 *	as buf is a stack-allocated string, the value will be changed in the unwound
 *	function as well.
 */
static gchar *
formatter_process_real(Formatter *f, const gchar *src, gchar *buf, gchar **values)
{
	const gchar *p;
	gchar *i;

	g_return_val_if_fail(f != NULL, NULL);
	g_return_val_if_fail(src != NULL, NULL);
	g_return_val_if_fail(buf != NULL, NULL);
	g_return_val_if_fail(values != NULL, NULL);

	i = buf;
	for (p = src; *p != '\0'; p++)
	{
		switch (*p)
		{
		case '%':
			switch (*(p + 1))
			{
			case 'b':
			case 'B':
				*i++ = '\x02';
				p++;
				break;
			case 'c':
			case 'C':
				*i++ = '\x03';
				p++;
				break;
			case 'h':
			case 'H':
				*i++ = prefs.indent_nicks ? '\x08' : ' ';
				p++;
				break;
			case 'o':
			case 'O':
				*i++ = '\x0F';
				p++;
				break;
			case 'u':
			case 'U':
				*i++ = '\x1F';
				p++;
				break;
			default:
				*i++ = *p;
				break;
			}
			break;
		case '$':
			p++;
			if (*p == 't')
				*i++ = '\t';
			else
			{
				gchar *vp;
				gint arg;

				arg = atoi(p);
				if (arg <= f->args)
				{
					for (vp = values[arg - 1]; *vp != '\0'; vp++)
						*i++ = *vp;
				}

				while (isdigit(*(p + 1)))
					p++;
			}
			break;
		default:
			*i++ = *p;
		}
	}
	*i++ = '\0';

	return buf;
}

gchar *
formatter_process(Formatter *f, gchar **data)
{
	gchar buf[4096];
	gchar *signame;

	g_return_val_if_fail(f != NULL, NULL);
	g_return_val_if_fail(data != NULL, NULL);

	signame = g_strdup_printf("format %s", f->key);
	signal_emit(signame, 2, f->args, data);
	g_free(signame);

	formatter_process_real(f, f->format, buf, data);

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
