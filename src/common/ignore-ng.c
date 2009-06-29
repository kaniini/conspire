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

ignore_entry *
ignore_exists(gchar *mask)
{
    g_return_val_if_fail(mask != NULL, NULL);

    return mowgli_dictionary_retrieve(ignores, mask);
}

gboolean
ignore_set(gchar *mask, gint32 levels)
{
    ignore_entry *temp = 0;
    gboolean change_only;

    temp = ignore_exists(mask);
    if (temp)
        change_only = TRUE;

    if (!change_only)
        temp = malloc(sizeof(ignore_entry));

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
ignore_show_entry(mowgli_dictionary_elem_t *element, gpointer data)
{
    ignore_entry *ignore = (ignore_entry *)element->data;
    gchar *ignoring = snprintf("%100s  ", 100, ignore->mask);
    session *sess = data;

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
            g_strconcat(ignoring, "dccs", NULL);
        if (ignore->levels & IGNORE_DCCMSGS)
            g_strconcat(ignoring, "dccmsgs", NULL);
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
ignore_del(gchar *mask)
{
    return mowgli_dictionary_delete(dict, mask);
}

