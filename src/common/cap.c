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

#include "cap.h"
#include "server.h"

CapState *
cap_state_new(server *serv, const gchar *caps)
{
	CapState *cap = g_slice_new(CapState);

	cap->serv = serv;
	cap->caps = g_strdup(caps);
	cap->refs = 1;

	return cap;
}

void
cap_state_ref(CapState *cap)
{
	cap->refs++;
}

void
cap_state_unref(CapState *cap)
{
	server *serv = cap->serv;

	cap->refs--;

	if (cap->refs == 0)
	{
		tcp_sendf(cap->serv, "CAP END\r\n");

		serv->cap = NULL;

		g_free(cap->caps);
		g_slice_free(CapState, cap);
	}
}
