/* Conspire
 * Copyright (C) 2009 Kiyoshi Aman.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdinc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xchat.h"
#include "ignore-ng.h"
#include "cfgfiles.h"
#include "fe.h"
#include "text.h"
#include "util.h"
#include "xchatc.h"
#include "signal_factory.h"
#include "command_factory.h"
#include "command_option.h"

mowgli_dictionary_t *ignores;

IgnoreEntry *
ignore_find_entry(const gchar *mask)
{
    g_return_val_if_fail(mask != NULL, NULL);

    return mowgli_dictionary_retrieve(ignores, mask);
}

gboolean
ignore_set(const gchar *mask, const IgnoreLevel levels)
{
    IgnoreEntry *temp = 0;

    temp = ignore_find_entry(mask);
    if (temp)
        ignore_del(mask);

    temp = g_slice_new0(IgnoreEntry);

    temp->mask   = g_strdup(mask);
    temp->levels = levels;
    temp->spec   = g_pattern_spec_new(mask);
    
    mowgli_dictionary_add(ignores, mask, temp);
    
    return TRUE;
}

gint
ignore_show_entry(mowgli_dictionary_elem_t *element, gpointer data)
{
    IgnoreEntry *ignore = (IgnoreEntry *)element->data;
    gchar *ignoring = 0;
    session *sess = data;
    snprintf(ignoring, 100, "%100s  ", ignore->mask);

    if (ignore->levels == IGNORE_ALL)
        g_strconcat(ignoring, "all", NULL);
    else
    {
        if (ignore->levels == IGNORE_EXCEPT)
            g_strconcat(ignoring, "exempt from: ", NULL);
        if (ignore->levels & IGNORE_PRIVATE)
            g_strconcat(ignoring, "private ", NULL);
        if (ignore->levels & IGNORE_PUBLIC)
            g_strconcat(ignoring, "publics ", NULL);
        if (ignore->levels & IGNORE_NOTICE)
            g_strconcat(ignoring, "notices ", NULL);
        if (ignore->levels & IGNORE_CTCP)
            g_strconcat(ignoring, "ctcps ", NULL);
        if (ignore->levels & IGNORE_ACTION)
            g_strconcat(ignoring, "actions ", NULL);
        if (ignore->levels & IGNORE_JOINS)
            g_strconcat(ignoring, "joins ", NULL);
        if (ignore->levels & IGNORE_PARTS)
            g_strconcat(ignoring, "parts ", NULL);
        if (ignore->levels & IGNORE_QUITS)
            g_strconcat(ignoring, "quits ", NULL);
        if (ignore->levels & IGNORE_KICKS)
            g_strconcat(ignoring, "kicks ", NULL);
        if (ignore->levels & IGNORE_MODES)
            g_strconcat(ignoring, "modes ", NULL);
        if (ignore->levels & IGNORE_TOPICS)
            g_strconcat(ignoring, "topics ", NULL);
        if (ignore->levels & IGNORE_INVITES)
            g_strconcat(ignoring, "invites ", NULL);
        if (ignore->levels & IGNORE_NICKS)
            g_strconcat(ignoring, "nicks", NULL);
        if (ignore->levels & IGNORE_DCC)
            g_strconcat(ignoring, "dccs ", NULL);
        if (ignore->levels & IGNORE_HILIGHT)
            g_strconcat(ignoring, "hilights", NULL);
    }
    signal_emit("ignore list entry", 2, sess, ignoring);

    return 0;
}

void
ignore_showlist(session *sess)
{
    signal_emit("ignore list header", 1, sess);
    mowgli_dictionary_foreach(ignores, ignore_show_entry, sess);
    signal_emit("ignore list footer", 1, sess);
}

gboolean
ignore_del(const gchar *mask)
{
    if (mowgli_dictionary_delete(ignores, mask))
        return TRUE;
    return FALSE;
}

IgnoreEntry *
ignore_check_entry(mowgli_dictionary_elem_t *element, IgnoreEntry *mask)
{
    IgnoreEntry *ignore = (IgnoreEntry *)element->data;

    if (g_pattern_match_string(ignore->spec, mask->mask)) {
        if (ignore->levels & mask->levels)
            return ignore;
    }
    return NULL;
}

gboolean
ignore_check(const gchar *mask, const IgnoreLevel levels)
{
    IgnoreEntry *ignore = g_slice_new0(IgnoreEntry);
    ignore->mask = mask;
    ignore->levels = levels;
    if (mowgli_dictionary_search(ignores, ignore_check_entry, ignore))
        return TRUE;
    return FALSE;
}

void
ignore_load(void)
{
    GError *error;
    gchar *filename = g_build_filename(get_xdir_fs(), "ignore.txt", NULL);
    GIOChannel *file = g_io_channel_new_file(filename, "r", &error);
    gchar *str;
    gchar **entries;
    IgnoreEntry *ignore;
    gchar *vp;
    gsize len;

    ignores = mowgli_dictionary_create(strcasecmp);

    if (error != NULL)
    {
        g_io_channel_close(file);
        return;
    }
    else
    {
        while (g_io_channel_read_line(file, &str, &len, NULL, &error))
        {
            if (error != NULL)
                g_io_channel_close(file);
                return;
            if (len < 1)
                continue;
            entries = g_strsplit(str, " = ", 0);
            ignore = g_slice_new0(IgnoreEntry);
            ignore->mask = entries[0];
            ignore->spec = g_pattern_spec_new(entries[0]);
            
            for (vp = entries[1]; *vp != '\0'; vp++)
            {
                ignore->levels *= 10;
                ignore->levels += g_ascii_digit_value(*vp);
            }
            mowgli_dictionary_add(ignores, ignore->mask, ignore);
        }
    }
    g_io_channel_close(file);
}

gint
ignore_save_entry(mowgli_dictionary_elem_t *element, GIOChannel *file)
{
    IgnoreEntry *ignore = (IgnoreEntry *)element->data;
    gchar *buf = 0;
    gsize bytes;
    GError *error;

    g_snprintf(buf, "%s = %d\n", 109, ignore->mask, ignore->levels);
    g_io_channel_write_chars(file, buf, sizeof(buf), &bytes, &error);
    if (error != NULL)
        return 1;
    return 0;
}

void
ignore_save(void)
{
    GError *error;
    gchar *filename = g_build_filename(get_xdir_fs(), "ignore.txt", NULL);
    GIOChannel *file = g_io_channel_new_file(filename, "r", &error);

    mowgli_dictionary_foreach(ignores, ignore_save_entry, file);

    g_io_channel_close(file);
}

CommandResult
cmd_ignore (struct session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
    gboolean except, private, public, notice, ctcp, action, joins;
    gboolean parts, quits, kicks, modes, topics, invites, nicks;
    gboolean dcc, dccmsgs, hilight, all;
    CommandOption options[] = {
        {"except",  TYPE_BOOLEAN, &except,  N_("User will %Bnot%B be ignored.")},
        {"private", TYPE_BOOLEAN, &private, N_("Private messages from the user will be ignored.")},
        {"public",  TYPE_BOOLEAN, &public,  N_("Public messages from the user will be ignored.")},
        {"notice",  TYPE_BOOLEAN, &notice,  N_("Notices from the user will be ignored.")},
        {"ctcp",    TYPE_BOOLEAN, &ctcp,    N_("CTCPs from the user will be ignored.")},
        {"action",  TYPE_BOOLEAN, &action,  N_("Actions, both public & private, from the user will be ignored.")},
        {"join",    TYPE_BOOLEAN, &joins,   N_("Join messages from the user will be ignored.")},
        {"part",    TYPE_BOOLEAN, &parts,   N_("Part messages from the user will be ignored.")},
        {"quit",    TYPE_BOOLEAN, &quits,   N_("Quit messages from the user will be ignored.")},
        {"kick",    TYPE_BOOLEAN, &kicks,   N_("Kicks from the user will be ignored.")},
        {"mode",    TYPE_BOOLEAN, &modes,   N_("Mode changes from the user will be ignored.")},
        {"topic",   TYPE_BOOLEAN, &topics,  N_("Topic changes from the user will be ignored.")},
        {"invite",  TYPE_BOOLEAN, &invites, N_("Invitations from the user will be ignored.")},
        {"nick",    TYPE_BOOLEAN, &nicks,   N_("Nick changes from the user will be ignored.")},
        {"dcc",     TYPE_BOOLEAN, &dcc,     N_("DCC file transfers from the user will be ignored.")},
        {"hilight", TYPE_BOOLEAN, &hilight, N_("Messages from the user that would otherwise hilight, won't.")},
        {"all",     TYPE_BOOLEAN, &all,     N_("All messages from the user will be ignored.")},
    };
    gint len = g_strv_length(word);
    IgnoreLevel levels = IGNORE_NONE;

    command_option_parse(sess, &len, &word, options);

    PrintTextf(sess, "Ignoring: %s", word[2]);

    if (except)
        levels += IGNORE_EXCEPT;
    if (private)
        levels += IGNORE_PRIVATE;
    if (public)
        levels += IGNORE_PUBLIC;
    if (notice)
        levels += IGNORE_NOTICE;
    if (ctcp)
        levels += IGNORE_CTCP;
    if (action)
        levels += IGNORE_ACTION;
    if (joins)
        levels += IGNORE_JOINS;
    if (parts)
        levels += IGNORE_PARTS;
    if (quits)
        levels += IGNORE_QUITS;
    if (kicks)
        levels += IGNORE_KICKS;
    if (modes)
        levels += IGNORE_MODES;
    if (topics)
        levels += IGNORE_TOPICS;
    if (invites)
        levels += IGNORE_INVITES;
    if (nicks)
        levels += IGNORE_NICKS;
    if (dcc)
        levels += IGNORE_DCC;
    if (all)
        levels = IGNORE_ALL - levels;
    if (ignore_set(word[2], levels))
    {
        signal_emit("ignore added", 2, sess, word[2]);
        return CMD_EXEC_OK;
    }
    else
        return CMD_EXEC_FAIL;
}

CommandResult
cmd_unignore(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
    gchar *mask = word[2];
    if (ignore_del(mask))
    {
        signal_emit("ignore removed", 2, sess, mask);
        return CMD_EXEC_OK;
    }
    return CMD_EXEC_FAIL;
}


static gboolean
flood_autodialog_timeout (gpointer data)
{
    prefs.autodialog = 1;
    return FALSE;
}

gint
flood_check (gchar *nick, gchar *ip, server *serv, session *sess, gint what)	/*0=ctcp  1=priv */
{
    /*
       serv
       int ctcp_counter; 
       time_t ctcp_last_time;
       prefs
       unsigned int ctcp_number_limit;
       unsigned int ctcp_time_limit;
       */
    gchar buf[512];
    gchar real_ip[132];
    gint i;
    time_t current_time;
    current_time = time (NULL);

    if (what == 0)
    {
        if (serv->ctcp_last_time == 0)	/*first ctcp in this server */
        {
            serv->ctcp_last_time = time (NULL);
            serv->ctcp_counter++;
        }
        else
        {
            /*if we got the ctcp in the seconds limit */
            if (difftime (current_time, serv->ctcp_last_time) < prefs.ctcp_time_limit)
            {
                serv->ctcp_counter++;
                /*if we reached the maximun numbers of ctcp in the seconds limits */
                if (serv->ctcp_counter == prefs.ctcp_number_limit)
                {
                    serv->ctcp_last_time = current_time;	/*we got the flood, restore all the vars for next one */
                    serv->ctcp_counter = 0;
                    for (i = 0; i < 128; i++)
                        if (ip[i] == '@')
                            break;
                    snprintf (real_ip, sizeof (real_ip), "*!*%s", &ip[i]);

                    snprintf (buf, sizeof (buf),
                            _("You are being CTCP flooded from %s, ignoring %s\n"),
                            nick, real_ip);
                    PrintText (sess, buf);

                    ignore_set(real_ip, IGNORE_CTCP);
                    return 0;
                }
            }
        }
    }
    else
    {
        if (serv->msg_last_time == 0)
        {
            serv->msg_last_time = time (NULL);
            serv->ctcp_counter++;
        }
        else
        {
            if (difftime (current_time, serv->msg_last_time) < prefs.msg_time_limit)
            {
                serv->msg_counter++;
                /*if we reached the maximun numbers of ctcp in the seconds limits */
                if (serv->msg_counter == prefs.msg_number_limit)
                {
                    snprintf (buf, sizeof (buf),
                            _("You are being MSG flooded from %s, setting gui_auto_open_dialog OFF and ignoring.\n"),
                            ip);

                    for (i = 0; i < 128; i++)
                        if (ip[i] == '@')
                            break;
                    snprintf (real_ip, sizeof (real_ip), "*!*%s", &ip[i]);
                    ignore_set(real_ip, IGNORE_PRIVATE);
                    PrintText (sess, buf);
                    /*we got the flood, restore all the vars for next one */
                    serv->msg_last_time = current_time;
                    serv->msg_counter = 0;
                    /*ignore_add (char *mask, int priv, int noti, int chan,
                      int ctcp, int invi, int unignore, int no_save) */

                    if (prefs.autodialog)
                    {
                        /*FIXME: only ignore ctcp or all?, its ignoring ctcps for now */
                        prefs.autodialog = 0;
                        /* turn it back on in 30 secs */
                        g_timeout_add (30000, flood_autodialog_timeout, NULL);
                    }
                    return 0;
                }
            }
        }
    }
    return 1;
}

