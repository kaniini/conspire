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

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/signal_factory.h"
#include "common/text.h"

extern session *current_sess; /* XXX */

static void
process_message(gpointer *params)
{
	session *sess  = params[0];
	gchar *from    = params[1];
	gchar *message = params[2];

	if (sess == current_sess)
		return;

	PrintTextf(current_sess, "\00323*\tYou have been highlighted on %s/%s by %s: %s",
		   sess->server->server_session->channel,
		   sess->channel ? sess->channel : "<unknown channel>",
		   from, message);
}

gboolean
init(Plugin *p)
{
	signal_attach("action public hilight", process_message);
	signal_attach("message public hilight", process_message);

	return TRUE;
}

gboolean
fini(Plugin *p)
{
	signal_disconnect("action public hilight", process_message);
	signal_disconnect("message public hilight", process_message);

	return TRUE;
}

PLUGIN_DECLARE("Highlight Notifier", PACKAGE_VERSION, 
	"Notifies you about highlights in your currently selected buffer.",
	"William Pitcock", init, fini);
