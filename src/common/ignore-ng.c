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

#include <glib/glib.h>
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
    gboolean change_only;

    temp = ignore_exists(mask);
    if (temp)
        change_only = TRUE;

    if (!change_only)
        temp = malloc(sizeof(IgnoreEntry));

    if (!temp)
        return FALSE;

    temp->mask = g_strdup(mask);
    temp->levels = levels;
    temp->spec = g_pattern_spec_new(mask);
    
    mowgli_dictionary_add(ignores, mask, temp);
    fe_ignore_update(1);
    
    return TRUE;
}

gint
ignore_show_entry(mowgli_dictionary_elem_t *element, session *sess)
{
    IgnoreEntry *ignore = (IgnoreEntry *)element->data;
    gchar *ignoring = snprintf("%100s  ", 100, ignore->mask);

    if (ignore->levels == IGNORE_NONE)
        g_strconcat(ignoring, "none", NULL);
    else if (ignore->levels == IGNORE_ALL)
        g_strconcat(ignoring, "all", NULL);
    else
    {
        if (ignore->levels & IGNORE_PRIVATE)
            g_strconcat(ignoring, "private ", NULL);
        if (ignore->levels & IGNORE_PUBLIC)
            g_strconcat(ignoring, "publics ", NULL);
        if (ignore->levels & IGNORE_NOTICE)
            g_strconcat(ignoring, "notices ", NULL);
        if (ignore->levels & IGNORE_SNOTES)
            g_strconcat(ignoring, "snotes ", NULL);
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
        if (ignore->levels & IGNORE_DCCMSGS)
            g_strconcat(ignoring, "dccmsgs ", NULL);
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
    return mowgli_dictionary_delete(dict, mask);
}

IgnoreEntry *
ignore_check_entry(mowgli_dictionary_elem_t *element, IgnoreEntry *mask)
{
    IgnoreEntry *ignore = (IgnoreEntry *)element->data;

    if (g_pattern_spec_match(ignore->spec, mask->mask)) {
        if (ignore->levels & mask->levels)
            return ignore;
    }
    return NULL;
}

gboolean
ignore_check(const gchar *mask, const IgnoreLevel levels)
{
    IgnoreEntry *ignore = {mask, levels, NULL};
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
    } else
    {
        while (g_io_channel_read_line(file, &str, &len;, NULL, &error))
        {
            if (error != NULL)
                g_io_channel_close(file);
                return;
            if (len < 1)
                continue;
            entries = g_strsplit(str, " = ", 0);
            ignore->mask = entries[0];
            ignore->spec = g_pattern_spec_new(entries[0]);
            
            for (vp = entries[1]; *vp != '\0'; *vp++)
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
    gchar *buf;
    gsize bytes;
    GError *error;

    g_snprintf(buf, "%s = %d\n", 109, ignore->mask, ignore->levels);
    g_io_channel_write_chars(file, buf, sizeof(buf), &bytes, &error);
    if (error != NULL)
        return 1;
}


void
ignore_save(void)
{
    GError *error;
    gchar *filename = g_build_filename(get_xdir_fs(), "ignore.txt", NULL);
    GIOChannel *file = g_io_channel_new_file(filename, "r", &error);
    gchar *str;
    IgnoreEntry *ignore;

    mowgli_dictionary_foreach(ignores, ignore_save_entry, file);

    g_io_channel_close(file);
}

