/*
 *  ircd-ratbox: A slightly useful ircd.
 *  ircd_getopt.h: A header for the getopt() command line option calls.
 *
 *  Copyright (C) 1990 Jarkko Oikarinen and University of Oulu, Co Center
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2004 ircd-ratbox development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *  $Id: ircd_getopt.h 6 2005-09-10 01:02:21Z nenolod $
 */

#ifndef CONSPIRE_GETOPT_H
#define CONSPIRE_GETOPT_H

typedef enum {
    TYPE_BOOLEAN = (0),
    TYPE_INTEGER = (1),
    TYPE_STRING  = (1 << 1),
    TYPE_USAGE   = (1 << 2),
} CommandOptionType;

typedef struct
{
    const gchar *opt;       /* name of the argument */
    CommandOptionType type; /* what type of option this is */
    gpointer argloc;        /* where we store the argument to it (-option argument) */
    const gchar *desc;      /* description of the argument, usage for printing help */
} CommandOption;

void command_option_usage(session *sess, gchar *name);
void command_option_parse(session *sess, gint *argc, gchar ***argv, CommandOption opts[]);

#endif /* CONSPIRE_GETOPT_H */
