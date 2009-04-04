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

#ifndef __CONSPIRE_PLUGIN_JAVASCRIPT_JAVASCRIPT_H__GUARD
#define __CONSPIRE_PLUGIN_JAVASCRIPT_JAVASCRIPT_H__GUARD

#define XP_UNIX			/* XXX */
#include "conspire-config.h"
#include <mowgli.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <dlfcn.h>
#include <string.h>
#include <mozjs/jsapi.h>

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/xchatc.h"
#include "common/server.h"
#include "common/outbound.h"
#include "common/text.h"

#include "session.h"
#include "commands.h"
#include "signals.h"

typedef struct {
	JSContext *context;
	JSObject *global;
	JSObject *functor;
} cjs_callback_t;

cjs_callback_t *cjs_callback_create(JSContext *, JSObject *, JSObject *);
void cjs_callback_free(cjs_callback_t *);

gboolean cjs_script_load(session *, const gchar *);
gboolean cjs_script_unload(session *, const gchar *);

#endif
