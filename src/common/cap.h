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

#include "xchat.h"

#ifndef __CONSPIRE__LIBCONSPIRE__CAP_H__GUARD
#define __CONSPIRE__LIBCONSPIRE__CAP_H__GUARD

struct server;

typedef struct {
	struct server *serv;
	gchar *caps;
	gint refs;
} CapState;

CapState *cap_state_new(struct server *serv, const gchar *caps);
void cap_state_ref(CapState *cap);
void cap_state_unref(CapState *cap);

#endif
