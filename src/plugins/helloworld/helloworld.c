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

/* outside of the tree, use <conspire/foo.h> instead. */
#include "common/plugin.h"
#include "common/xchat.h"
#include "common/command_factory.h"
#include "common/outbound.h"
#include "common/text.h"

/*
 * sess = session (old xchat_context)
 * tbuf = stack buffer for doing fast string manipulation on (may be removed in 0.30)
 * word = same as xchat except offset from 0 not 1 like normal C arrays
 * word_eol = ditto
 *
 * see command_factory.h for more information (you can read C code right?)
 */
CommandResult
cmd_hello(session *sess, char *tbuf, const gchar *word[], const gchar *word_eol[])
{
	handle_command(sess, "SAY testing 1 2 3", FALSE);	/* FALSE = don't complain to user about failures */

	return CMD_EXEC_OK;
}

gboolean
init(Plugin *p)
{
	command_register("HELLO", "Hello World example", 0, cmd_hello);
	return TRUE;
}

gboolean
fini(Plugin *p)
{
	command_remove_handler("HELLO", cmd_hello);
	return TRUE;
}

PLUGIN_DECLARE("Hello World example", PACKAGE_VERSION,
	"Example /HELLO command implementation.",
	"William Pitcock", init, fini);
