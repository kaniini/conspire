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

#define _GNU_SOURCE	/* for memrchr */
#include <stdlib.h>
#include <string.h>

#include <mowgli.h>

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/command_factory.h"
#include "common/outbound.h"
#include "common/text.h"
#include "common/fe.h"
#include "common/util.h"

#define EXEC_HELP "Usage: EXEC [-o] [-d] <command>"

struct exec_process
{
	pid_t pid;
	int io_tag;
	int descriptor;
	gboolean option_shell;
	gboolean option_irc_output;
	struct {
		char *text;
		size_t length;
		size_t start;
	} buffer;
	struct session *session;
};

mowgli_heap_t *exec_process_heap = NULL;
mowgli_list_t *exec_processes = NULL;

static void
exec_process_cancel(struct exec_process *process, int signal)
{
	kill(process->pid, signal);
}

static struct exec_process *
exec_process_create()
{
	struct exec_process *process = mowgli_heap_alloc(exec_process_heap);
	process->option_irc_output = FALSE;
	process->option_shell = TRUE;
	process->pid = (pid_t)-1;
	process->descriptor = -1;
	process->io_tag = -1;
	process->session = NULL;
	process->buffer.text = NULL;
	process->buffer.length = 0;
	process->buffer.start = 0;

	return process;
}

static void
exec_process_destroy(struct exec_process *process)
{
	close(process->descriptor);
	if(process->buffer.text)
		free(process->buffer.text);

	mowgli_heap_free(exec_process_heap, process);
}

#ifndef HAVE_MEMRCHR
static void *
memrchr(const void *block, int c, size_t size)
{
	unsigned char *p;

	for (p = (unsigned char *)block + size; p != block; p--)
		if (*p == c)
			return p;
	return 0;
}
#endif

static void
exec_data_print(struct exec_process *process)
{
	if(process->buffer.length > 0) {
		if(process->option_irc_output)
		{
			handle_multiline_raw(process->session, process->buffer.text + process->buffer.start);
		}
		else
			PrintText(process->session, process->buffer.text + process->buffer.start);
	}
}

static gboolean
exec_data(GIOChannel *source, GIOCondition condition, struct exec_process *process)
{
	ssize_t size;
	char *rest = NULL;

	if(process->buffer.length) {
		char *bt;

		bt = realloc(process->buffer.text, process->buffer.length + (sizeof(char) * 2048));
		process->buffer.text = bt;
	}
	else
		process->buffer.text = malloc(sizeof(char) * 2048);

	size = read(process->descriptor, process->buffer.text + (sizeof(char) * process->buffer.length), 2047);
	if(size < 1)
	{
		/* The process has died */
		kill(process->pid, SIGKILL);

		process->buffer.text[process->buffer.length] = '\0';
		exec_data_print(process);

		waitpid(process->pid, NULL, 0);
		g_source_remove(process->io_tag);

		mowgli_node_t *node = mowgli_node_find(process, exec_processes);

		exec_process_destroy(process);

		mowgli_node_delete(node, exec_processes);
		mowgli_node_free(node);

		return CMD_EXEC_OK;
	}

	process->buffer.length += size;
	process->buffer.text[process->buffer.length] = '\0';

	rest = memrchr(process->buffer.text, '\n', process->buffer.length);
	if(rest)
		*rest = '\0';

	exec_data_print(process);

	if(rest)
		process->buffer.start = process->buffer.text - rest + 1;

	return CMD_EXEC_OK;
}

static CommandResult
exec_cmd_exec(session *sess, char *tbuf, char *word[], char *word_eol[])
{
	char *cmd = word_eol[2];

	if (*cmd)
	{
		int offset = 0;

		struct exec_process *process;

		int out[2];
		pid_t pid;
		gboolean option_irc_output = FALSE;
		gboolean option_shell = TRUE;

		if(strcmp(word[2], "-o") == 0) {
			option_irc_output = TRUE;
			cmd = word_eol[3];
			offset++;
		}
		if(strcmp(word[2 + offset], "-d") == 0) {
			option_shell = FALSE;
			cmd = word_eol[3 + offset];
			offset++;
		}

		if(!*word[2 + offset])
			return CMD_EXEC_FAIL;

		if (option_shell)
		{
			if (access("/bin/sh", X_OK) != 0)
			{
				fe_message(_("I need /bin/sh to run!\n"), FE_MSG_ERROR);
				return CMD_EXEC_FAIL;
			}
		}

#ifdef __EMX__						  /* if os/2 */
		if (pipe(out) < 0)
		{
			PrintText(sess, "Pipe creation error\n");
			return CMD_EXEC_FAIL;
		}
		setmode(out[0], O_BINARY);
		setmode(out[1], O_BINARY);
#else
		if (socketpair(PF_UNIX, SOCK_STREAM, 0, out) == -1)
		{
			PrintText(sess, "socketpair(2) failed\n");
			return CMD_EXEC_FAIL;
		}
#endif

		process = exec_process_create();
		process->session = sess;
		process->descriptor = out[0];
		process->option_irc_output = option_irc_output;
		process->option_shell = option_shell;

		pid = fork();
		if(pid == 0)
		{
			/* This is the child's context */
			close(0);
			close(1);
			close(2);

			/* Close parent's end of pipe */
			close(process->descriptor);

			/* Copy the child end of the pipe to stdout and stderr */
			dup2(out[1], STDOUT_FILENO);
			dup2(out[1], STDERR_FILENO);

			/* Now close all open file descriptors except stdin, stdout and stderr */
			{
				int fd;
				for(fd = 3; fd < 1024; fd++) close(fd);
			}

			/* Now we call /bin/sh to run our cmd ; made it more friendly -DC1 */
			if(process->option_shell)
			{
				execl("/bin/sh", "sh", "-c", cmd, NULL);
			}
			else
			{
				char **argv;
				int argc;

				my_poptParseArgvString(cmd, &argc, &argv);
				execvp(argv[0], argv);
			}

			/* not reached unless error */
			fflush(stdout);
			fflush(stdin);
			_exit(-1);
		}
		else if(pid == -1)
		{
			/* Parent context; fork() failed. */

			PrintText(sess, "Error in fork(2)\n");
			close(out[0]);
			close(out[1]);

			exec_process_destroy(process);
		}
		else
		{
			/* Parent path. */
			close(out[1]);
			process->pid = pid;
			process->io_tag = fe_input_add(process->descriptor, FIA_READ | FIA_EX, exec_data, process);

			mowgli_node_add(process, mowgli_node_create(), exec_processes);
			return CMD_EXEC_OK;
		}
	}

	return CMD_EXEC_FAIL;
}

static void
exec_sig_session_destroy(gpointer *params)
{
	struct session *sess = params[0];

	mowgli_node_t *n, *tn;
	struct exec_process *process;

	MOWGLI_LIST_FOREACH_SAFE(n, tn, exec_processes->head) {
		process = (struct exec_process *)n->data;

		if(process->session == sess) {
			/* Forget about it. Alternatively, consider redirecting the output
			 * to the current session instead.
			 */
			exec_process_cancel(process, SIGKILL);
			exec_process_destroy(process);

			mowgli_node_delete(n, exec_processes);
			mowgli_node_free(n);
		}
	}
}

gboolean
exec_initialize(Plugin *p)
{
	if(!exec_processes) {
		exec_processes = mowgli_list_create();
		exec_process_heap = mowgli_heap_create(sizeof(struct exec_process), 32, BH_LAZY);
	}

	/* Commands. */
	command_register("EXEC", N_(EXEC_HELP), CMD_NO_FLAGS, exec_cmd_exec);

	/* Signals. */
	signal_attach("session destroy", exec_sig_session_destroy);
	return TRUE;
}

gboolean
exec_finalize(Plugin *p)
{
	if(MOWGLI_LIST_LENGTH(exec_processes) > 0) {
		mowgli_node_t *n, *tn;

		MOWGLI_LIST_FOREACH(n, exec_processes->head) {
			exec_process_cancel((struct exec_process *)n->data, SIGTERM);
		}

		/* Give it a second. */
		sleep(1);

		MOWGLI_LIST_FOREACH(n, exec_processes->head) {
			exec_process_cancel((struct exec_process *)n->data, SIGKILL);
		}

		MOWGLI_LIST_FOREACH_SAFE(n, tn, exec_processes->head) {
			exec_process_destroy((struct exec_process *)n->data);

			mowgli_node_delete(n, exec_processes);
			mowgli_node_free(n);
		}
	}

	/* Commands. */
	command_remove_handler("EXEC", exec_cmd_exec);

	/* Signals. */
	signal_disconnect("session destroy", exec_sig_session_destroy);

	return TRUE;
}

PLUGIN_DECLARE("exec", PACKAGE_VERSION,
	"Support for executing external commands", "Noah Fontes",
	exec_initialize, exec_finalize);
