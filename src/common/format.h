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

#ifndef __LIBCONSPIRE__COMMON__FORMAT__H__GUARD
#define __LIBCONSPIRE__COMMON__FORMAT__H__GUARD

typedef struct {
	gchar *key;
	gchar *format;
	int args;
} Formatter;

Formatter *formatter_register(const gchar *key, const gchar *format, int args);
Formatter *formatter_get(const gchar *key);
gchar *formatter_process(Formatter *f, gchar **data);
void formatter_remove(const gchar *key);

extern mowgli_dictionary_t *formatters;

#endif
