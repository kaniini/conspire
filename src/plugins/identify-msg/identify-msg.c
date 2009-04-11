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
#include "common/conspire-config.h"
#include "common/xchat.h"
#include "common/signal_factory.h"
#include "common/text.h"

extern session *current_sess; /* XXX */

static void
process_cap(gpointer *params)
{
	CapState *cap = params[0];
	server *serv  = cap->serv;

	switch (cap->op)
	{
	case CAP_LS:
		if (strstr(cap->caps, "identify-msg"))
			cap_add_cap(cap, "identify-msg");
		break;
	case CAP_ACK:
		if (strstr(cap->caps, "identify-msg"))
			serv->have_idmsg = TRUE;
		break;
	default:
		break;
	}
}

gboolean
init(Plugin *p)
{
	signal_attach("cap message", process_cap);

	return TRUE;
}

gboolean
fini(Plugin *p)
{
	signal_disconnect("cap message", process_cap);

	return TRUE;
}

PLUGIN_DECLARE("IDENTIFY-MSG Support", PACKAGE_VERSION, 
	"Support for identify-msg extension.",
	"William Pitcock", init, fini);
