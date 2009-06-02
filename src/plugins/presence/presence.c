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

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/signal_factory.h"
#include "common/text.h"
#include "common/userlist.h"

/* enable presence capability. */
static void
process_cap(gpointer *params)
{
	CapState *cap = params[0];

	switch(cap->op)
	{
	case CAP_LS:
		if (strstr(cap->caps, "presence"))
			cap_add_cap(cap, "presence");
		break;
	default:
		break;
	}
}

/* this translates the various presence changesets into the "presence changed" signal. */
static void
process_numeric_792(gpointer *params)
{
	session *sess    = params[0];
	gchar **word     = params[1];
	gchar **word_eol = params[2];

	g_return_if_fail(sess != NULL);
	g_return_if_fail(word != NULL);
	g_return_if_fail(word_eol != NULL);

	signal_emit("presence changed", 4, sess, word[4], word[5], word_eol[6] + 1);
}

gboolean
init(Plugin *p)
{
	signal_attach("cap message", process_cap);
	signal_attach("server numeric 792", process_numeric_792);

	return TRUE;
}

gboolean
fini(Plugin *p)
{
	signal_disconnect("cap message", process_cap);
	signal_disconnect("server numeric 792", process_numeric_792);

	return TRUE;
}

PLUGIN_DECLARE("IRCv3 Presence Extensions", PACKAGE_VERSION, 
	"Provides support for the IRCv3 Presence Protocol.",
	"William Pitcock", init, fini);
