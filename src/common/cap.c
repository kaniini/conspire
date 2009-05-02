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

static CapOperation
_cap_token_to_op(const gchar *token)
{
	static struct {
		const gchar *token;
		const CapOperation op;
	} ops[] = {
		{ "LS", CAP_LS },
		{ "ACK", CAP_ACK },
		{ "NAK", CAP_NAK }
	};

	int i;

	g_return_val_if_fail(token != NULL, CAP_NONE);

	for (i = 0; i < (sizeof(ops) / sizeof(*ops)); i++)
		if (!g_ascii_strcasecmp(token, ops[i].token))
			return ops[i].op;

	return CAP_NONE;
}

CapState *
cap_state_new(server *serv, const gchar *token, const gchar *caps)
{
	CapState *cap = g_slice_new0(CapState);

	cap->serv = serv;
	cap->op = _cap_token_to_op(token);
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
		switch (cap->op)
		{
		case CAP_NAK:
		case CAP_ACK:
			tcp_sendf(cap->serv, "CAP END");
			break;

		case CAP_LS:
			return cap_request(cap);
			break;

		default:
			break;
		}


		serv->cap = NULL;

		g_free(cap->caps);
		g_slice_free(CapState, cap);
	}
}

void
cap_add_cap(CapState *cap, const gchar *token)
{
	g_strlcat(cap->caps_request, token, sizeof(cap->caps_request));
	g_strlcat(cap->caps_request, " ", sizeof(cap->caps_request));
}

void
cap_request(CapState *cap)
{
	tcp_sendf_now(cap->serv, "CAP REQ :%s", cap->caps_request);
}
