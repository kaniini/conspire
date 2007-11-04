/*
 * Conspire
 * Copyright (C) 2007 William Pitcock, Kiyoshi Aman
 *
 * conspire-xchatwrap: Run some xchat plugins inside a secure conspire 
 *    sandbox.
 *
 * Based on:
 *    example.c - program to demonstrate some D-BUS stuffs.
 *    Copyright (C) 2006 Claessens Xavier
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Claessens Xavier
 * xclaesse@gmail.com
 */

#include "../../config.h"
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include "marshallers.h"

#include <glib.h>
#include <gmodule.h>

#define DBUS_SERVICE "org.atheme.conspire"
#define DBUS_REMOTE "/org/atheme/conspire"
#define DBUS_REMOTE_INTERFACE "org.atheme.conspire"

guint command_id;
guint server_id;

int
main (int argc, char **argv)
{
	GMainLoop *mainloop;
	GModule *mod;
	void (*sym_getinfo)(gpointer, char **, char **, char **, char *) = NULL;
	char *ver, *desc, *name;

	if (argc < 2)
	{
		g_print("usage: %s xchatplugin.so\n", argv[0]);
		return EXIT_SUCCESS;
	}

	xchatemu_init();

	mod = g_module_open(argv[1], G_MODULE_BIND_LAZY);
	g_module_symbol(mod, "xchat_plugin_init", (gpointer) &sym_getinfo);
	if (sym_getinfo == NULL)
	{
		g_print("Not a valid XChat plugin: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	else
		sym_getinfo(NULL, &name, &desc, &ver, NULL);

	g_print("XChat API emulation is very incomplete. Expect bugs.\n");
	xchat_plugingui_add(NULL, argv[1], name, desc, ver, NULL);

	mainloop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (mainloop);

	return EXIT_SUCCESS;
}
