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

#ifndef __CONSPIRE_COMMAND_FACTORY_H__GUARD
#define __CONSPIRE_COMMAND_FACTORY_H__GUARD

typedef enum {
    CMD_NO_FLAGS = 0,
    CMD_HANDLE_QUOTES = (1 << 1),
    CMD_NEED_CHANNEL = (1 << 2),
    CMD_NEED_SERVER = (1 << 3),
    CMD_STOP_ON_FAIL = (1 << 4),
} CommandFlags;

typedef struct {
    const gchar *description;
    const gchar *helptext;
    CommandFlags flags;
    GList *handlers;
} Command;

typedef enum {
    CMD_EXEC_OK,
    CMD_EXEC_FAIL,
    CMD_EXEC_STOP
} CommandResult;

typedef enum {
    COMMAND_EXEC_OK,
    COMMAND_EXEC_FAILED,
    COMMAND_EXEC_NOCMD
} CommandExecResult;

typedef CommandResult (*CommandHandler)(struct session * sess, char *tbuf, char *word[], char *word_eol[]);

void command_register(const gchar *name, const gchar *description, const gchar *helptext, CommandFlags flags, CommandHandler handler);
void command_remove_handler(const gchar *name, CommandHandler handler);
void command_set_flags(const gchar *name, CommandFlags flags);
CommandFlags command_get_flags(const gchar *name);
CommandExecResult command_execute(struct session *sess, const gchar *name, char *tbuf, char *word[], char *word_eol[]);

#endif
