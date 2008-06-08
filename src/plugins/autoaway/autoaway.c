/* autoaway.c
 *
 * An auto away plugin for xchat-gnome.
 *
 * Copyright (C) 2005 Isak Savo <isak.savo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <config.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <dlfcn.h>
#include <string.h>

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/server.h"
#include "common/outbound.h"

#include "autoaway.h"
#include "gscreensaver.h"
#include "xscreensaver.h"

static gint timeout_tag = 0;

typedef enum {
	STATE_ACTIVE,
	STATE_AWAY
} aa_state_t;

typedef enum {
	SS_NONE,	/* This plugin is useless if this is set		*/
	SS_GNOME,	/* Use DBus to talk to gnome-screensaver		*/
	SS_X11		/* We're talking to xscreensaver			*/
} screensaver_t;

static screensaver_t screensaver_type = SS_NONE;
static aa_state_t state = STATE_ACTIVE;

static screensaver_t
get_screensaver_type (void)
{
	if (get_gs_has_ipc ())
		return SS_GNOME;

	if (get_xss_has_ipc ())
		return SS_X11;

	return SS_NONE;
}

/* Use whatever available resource to find if the screensaver has kicked in or not */
static gboolean
get_screensaver_active (void)
{
	switch (screensaver_type) {
		case SS_X11:
			return get_xss_screensaver_active();

		case SS_GNOME:
			return get_gs_screensaver_active();

		case SS_NONE:
			return FALSE;
	}

	return FALSE;
}


/* Go away on all networks and do a nickname change if desired */
static void
toggle_away(void)
{
	GSList *node = serv_list;

	while (node)
	{
		server *serv = node->data;
		handle_command(serv->front_session, "AWAY", FALSE);

		node = g_slist_next(node);
	}
}

/*** Callbacks ***/

static int
timeout_cb (gpointer user_data)
{
	switch (state) {
		case STATE_ACTIVE:
			if (get_screensaver_active ()) {
				if (state != STATE_AWAY)
					toggle_away();
				state = STATE_AWAY;
			}
			break;

		case STATE_AWAY:
			if (get_screensaver_active () == FALSE) {
				if (state != STATE_ACTIVE)
					toggle_away();
				state = STATE_ACTIVE;
			}
			break;

		default:
			break;
	}

	return 1;
}

/*** xchat plugin functions ***/
gboolean
init(Plugin *p)
{
	init_gs_connection ();

	/* Hook up our callbacks. */
	timeout_tag = g_timeout_add_seconds(5, timeout_cb, NULL);

	screensaver_type = get_screensaver_type ();

	/* All done */

	/* FIXME: Perhaps return FALSE if we failed to find a running screensaver? */
	return TRUE;
}

gboolean
fini(Plugin *p)
{
	g_source_remove(timeout_tag);
	close_gs_connection ();

	return TRUE;
}

PLUGIN_DECLARE("Automatic Away", PACKAGE_VERSION, "Isak Savo and William Pitcock", init, fini);
