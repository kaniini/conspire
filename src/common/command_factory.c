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

#include "command_factory.h"

static mowgli_dictionary_t *cmd_dict_ = NULL;

void
command_register(const gchar *name, const gchar *helptext, CommandFlags flags, CommandHandler handler)
{
	Command *cmd;

	if (cmd_dict_ == NULL)
		cmd_dict_ = mowgli_dictionary_create(g_ascii_strcasecmp);

	if (!(cmd = mowgli_dictionary_retrieve(cmd_dict_, name)))
	{
		cmd = g_slice_new0(Command);
		cmd->helptext = g_strdup(helptext);
		cmd->flags = flags;
	}

	g_assert(cmd != NULL);

	cmd->handlers = g_list_append(cmd->handlers, handler);
}

void
command_remove_handler(const gchar *name, CommandHandler handler)
{
	Command *cmd;

	if (!(cmd = mowgli_dictionary_retrieve(cmd_dict_, name)))
		return;

	cmd->handlers = g_list_remove(cmd->handlers, handler);
}

void
command_set_flags(const gchar *name, CommandFlags flags)
{
	Command *cmd;

	if (!(cmd = mowgli_dictionary_retrieve(cmd_dict_, name)))
		return;

	cmd->flags = flags;
}

CommandFlags
command_get_flags(const gchar *name)
{
	Command *cmd;

	if (!(cmd = mowgli_dictionary_retrieve(cmd_dict_, name)))
		return CMD_NO_FLAGS;

	return cmd->flags;
}

CommandExecResult
command_execute(struct session *sess, const gchar *name, char *tbuf, char *word[], char *word_eol[])
{
	Command *cmd;
	GList *node;

	if (!(cmd = mowgli_dictionary_retrieve(cmd_dict_, name)))
		return COMMAND_EXEC_NOCMD;

	MOWGLI_ITER_FOREACH(node, cmd->handlers)
	{
		CommandResult ret;
		CommandHandler handler;

		handler = (CommandHandler) node->data;
		ret = handler(sess, tbuf, word, word_eol);

		if (ret == CMD_EXEC_FAIL)
			return COMMAND_EXEC_FAILED;

		if (ret == CMD_EXEC_STOP)
			break;
	}

	return COMMAND_EXEC_OK;
}

Command *
command_lookup(const gchar *name)
{
	g_return_val_if_fail(name != NULL, NULL);

	return mowgli_dictionary_retrieve(cmd_dict_, name);	
}
