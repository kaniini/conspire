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

#include <mowgli.h>
#include <gmodule.h>

#ifndef __CONSPIRE__LIBCONSPIRE__PLUGIN_H__GUARD
#define __CONSPIRE__LIBCONSPIRE__PLUGIN_H__GUARD

struct _Plugin;
struct _PluginHeader;

struct _Plugin {
	GModule *handle;
	struct _PluginHeader *header;
};

struct _PluginHeader {
	const gchar *name;
	const gchar *version;
	const gchar *author;
	gboolean (*init)(struct _Plugin *);
	gboolean (*fini)(struct _Plugin *);
};

typedef struct _Plugin Plugin;
typedef struct _PluginHeader PluginHeader;

#define PLUGIN_DECLARE(...) \
	static PluginHeader plugin = { \
		__VA_ARGS__ \
	}; \
	G_MODULE_EXPORT PluginHeader *conspire_get_plugin(void) { \
		return &plugin; \
	}

extern mowgli_dictionary_t *plugin_dict;

void plugin_load(const gchar *filename);
void plugin_close(const gchar *filename);
void plugin_autoload(void);

#endif
