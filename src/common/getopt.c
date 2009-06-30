/*
 *  ircd-ratbox: A slightly useful ircd.
 *  getopt.c: Uses getopt to fetch the command line options.
 *
 *  Copyright (C) 1990 Jarkko Oikarinen and University of Oulu, Co Center
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2005 ircd-ratbox development team
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
 *  $Id: getopt.c 6 2005-09-10 01:02:21Z nenolod $
 */

#include <glib.h>

#include "stdinc.h"
#include "signal_factory.h"
#include "xchat.h"
#include "text.h"
#include "getopt.h"

#define OPTCHAR '-'

void
command_option_parse(session *sess, gint *argc, gchar ***argv, CommandOption opts[])
{
    gint i;
    gchar *command = (*argv)[0];

    /* loop through each argument */
    for (;;)
    {
        gint found = 0;

        (*argc)--;
        (*argv)++;

        if(*argc < 1)
        {
            return;
        }

        /* check if it *is* an arg.. */
        if((*argv)[0][0] != OPTCHAR)
        {
            return;
        }

        (*argv)[0]++;

        /* search through our argument list, and see if it matches */
        for (i = 0; opts[i].opt; i++)
        {
            if(!strcmp(opts[i].opt, (*argv)[0]))
            {
                /* found our argument */
                found = 1;

                switch (opts[i].type)
                {
                    case TYPE_BOOLEAN:
                        *((int *) opts[i].argloc) = 1;
                        break;
                    case TYPE_INTEGER:
                        if(*argc < 2)
                        {
                            PrintTextf(sess, "Error: option '%c%s' requires an argument", OPTCHAR, opts[i].opt);
                            command_option_usage(sess, command, opts);
                            return;
                        }

                        *((int *) opts[i].argloc) = atoi((*argv)[1]);

                        (*argc)--;
                        (*argv)++;
                        break;
                    case TYPE_STRING:
                        if(*argc < 2)
                        {
                            PrintTextf(sess, "Error: option '%c%s' requires an argument", OPTCHAR, opts[i].opt);
                            command_option_usage(sess, command, opts);
                            return;
                        }

                        *((char **) opts[i].argloc) =
                            malloc(strlen((*argv)[1]) + 1);
                        strcpy(*((char **) opts[i].argloc), (*argv)[1]);

                        (*argc)--;
                        (*argv)++;
                        break;

                    case TYPE_USAGE:
                        command_option_usage(sess, command, opts);
                    /*NOTREACHED*/ default:
                        PrintTextf(sess,
                                "Error: internal error in command_option_parse at %s:%d",
                                __FILE__, __LINE__);
                        return;
                }
            }
        }
        if(!found)
        {
            PrintTextf(sess, "Error: unknown argument '%c%s'", OPTCHAR, (*argv)[0]);
            command_option_usage(sess, command, opts);
        }
    }
}

void
command_option_usage(session *sess, gchar *name, CommandOption opts[])
{
    gint i = 0;
    gchar *temp;

    PrintTextf(sess, "Usage: %s [options]", name);
    PrintText(sess, "Where valid options are:");

    for (i = 0; opts[i].opt; i++)
    {
        if ((opts[i].type == TYPE_BOOLEAN) || (opts[i].type == TYPE_USAGE))
            temp = "";
        else if (opts[i].type == TYPE_INTEGER)
            temp = "<number>";
        else if (opts[i].type == TYPE_STRING)
            temp = "<string>";
        else
            temp = "<unknown type>";

        PrintTextf(sess, "\t%c%-10s %-20s%s", OPTCHAR, opts[i].opt, temp, opts[i].desc);
    }
}
