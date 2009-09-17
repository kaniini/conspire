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

#ifndef __CONSPIRE_SIGNAL_FACTORY_H__GUARD
#define __CONSPIRE_SIGNAL_FACTORY_H__GUARD

typedef struct {
	const gchar *name;
	gboolean stop;
	GList *handlers;
	gpointer *values;
} Signal;

typedef void (*SignalHandler)(gpointer *params);

void signal_attach(const gchar *signal, SignalHandler hdl);
void signal_attach_head(const gchar *signal, SignalHandler hdl);

gint signal_emit(const gchar *signal, int params, ...);
void signal_continue(int params, ...);

void signal_stop(const gchar *signal);
void signal_stop_current(void);
const gchar *signal_get_current_name(void);

void signal_disconnect(const gchar *signal, SignalHandler hdl);

#endif

