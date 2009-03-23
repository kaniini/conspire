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

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/command_factory.h"
#include "common/outbound.h"
#include "common/text.h"

static GSList *timer_list = NULL;

#define STATIC
#define HELP \
"Usage: TIMER [-refnum <num>] [-repeat <num>] <seconds> <command>\n" \
"       TIMER [-quiet] -delete <num>"

typedef struct
{
	int tag;
	session *sess;
	char *command;
	int ref;
	int repeat;
	float timeout;
	unsigned int forever:1;
} timer;

static void
timer_del (timer *tim)
{
	g_source_remove(tim->tag);
	timer_list = g_slist_remove (timer_list, tim);
	free (tim->command);
	free (tim);
}

static void
timer_del_ref (session *sess, int ref, int quiet)
{
	GSList *list;
	timer *tim;

	list = timer_list;
	while (list)
	{
		tim = list->data;
		if (tim->ref == ref)
		{
			if (!quiet)
				PrintTextf (sess, "Timer %d deleted.\n", ref);
			timer_del (tim);
			return;
		}
		list = list->next;
	}
	if (!quiet)
		PrintText (sess, "No such ref number found.\n");
}

static int
timeout_cb (timer *tim)
{
	handle_command (tim->sess, tim->command, FALSE);

	if (tim->forever)
		return 1;

	tim->repeat--;
	if (tim->repeat > 0)
		return 1;

	timer_del (tim);
	return 0;
}

static void
timer_add (session *sess, int ref, float timeout, int repeat, char *command)
{
	timer *tim;
	GSList *list;

	if (ref == 0)
	{
		ref = 1;
		list = timer_list;
		while (list)
		{
			tim = list->data;
			if (tim->ref >= ref)
				ref = tim->ref + 1;
			list = list->next;
		}
	}

	tim = malloc (sizeof (timer));
	tim->ref = ref;
	tim->repeat = repeat;
	tim->timeout = timeout;
	tim->command = strdup (command);
	tim->sess    = sess;
	tim->forever = FALSE;

	if (repeat == 0)
		tim->forever = TRUE;

	tim->tag = g_timeout_add (timeout * 1000.0, (void *)timeout_cb, tim);
	timer_list = g_slist_append (timer_list, tim);
}

static void
timer_showlist (session *sess)
{
	GSList *list;
	timer *tim;

	if (timer_list == NULL)
	{
		PrintText (sess, "No timers installed.\n");
		return;
	}
							 /*  00000 00000000 0000000 abc */
	PrintText (sess, "\026 Ref#  Seconds  Repeat  Command \026\n");
	list = timer_list;
	while (list)
	{
		tim = list->data;
		PrintTextf (sess, "%5d %8.1f %7d  %s\n", tim->ref, tim->timeout,
		    tim->repeat, tim->command);
		list = list->next;
	}
}

CommandResult
cmd_timer (session *sess, char *tbuf, char *word[], char *word_eol[])
{
	int repeat = 1;
	float timeout;
	int offset = 0;
	int ref = 0;
	int quiet = FALSE;
	char *command;

	if (!word[2][0])
	{
		timer_showlist (sess);
		return CMD_EXEC_OK;
	}

	if (strcasecmp (word[2], "-quiet") == 0)
	{
		quiet = TRUE;
		offset++;
	}

	if (strcasecmp (word[2 + offset], "-delete") == 0)
	{
		timer_del_ref (sess, atoi (word[3 + offset]), quiet);
		return CMD_EXEC_OK;
	}

	if (strcasecmp (word[2 + offset], "-refnum") == 0)
	{
		ref = atoi (word[3 + offset]);
		offset += 2;
	}

	if (strcasecmp (word[2 + offset], "-repeat") == 0)
	{
		repeat = atoi (word[3 + offset]);
		offset += 2;
	}

	timeout = atof (word[2 + offset]);
	command = word_eol[3 + offset];

	if (timeout < 0.1 || !command[0])
		return CMD_EXEC_FAIL;

	timer_add (sess, ref, timeout, repeat, command);

	return CMD_EXEC_OK;
}

gboolean
init(Plugin *p)
{
	command_register("TIMER", HELP, 0, cmd_timer);
	return TRUE;
}

gboolean
fini(Plugin *p)
{
	command_remove_handler("TIMER", cmd_timer);
	return TRUE;
}

PLUGIN_DECLARE("Timer", PACKAGE_VERSION,
	"A timer plugin for Conspire",
	"Kiyoshi Aman", init, fini);

