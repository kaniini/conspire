/* Conspire
 * Copyright (C) 2007 William Pitcock
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
#include <glib-object.h>

#include "../common/xchat.h"
#include "../common/fe.h"
#include "../common/util.h"
#include "../common/text.h"
#include "../common/cfgfiles.h"
#include "../common/xchatc.h"

/* === command-line parameter parsing : requires glib 2.6 === */

static int arg_show_version = 0;

static const GOptionEntry gopt_entries[] =
{
 {"url",         0,  0, G_OPTION_ARG_STRING,    &arg_url, N_("Open an irc://server:port/channel URL"), "URL"},
 {"version",    'v', 0, G_OPTION_ARG_NONE,      &arg_show_version, N_("Show version information"), NULL},
 {NULL}
};

int
fe_args(int argc, char *argv[])
{
        GError *error = NULL;
        GOptionContext *context;

	/* nothing for now */
	g_type_init();

	arg_dont_autoconnect = 1;

        context = g_option_context_new (NULL);
        g_option_context_add_main_entries (context, gopt_entries, GETTEXT_PACKAGE);
        g_option_context_parse (context, &argc, &argv, &error);

        if (error)
        {
                if (error->message)
                        printf ("%s\n", error->message);
                return 1;
        }

        g_option_context_free (context);

	return -1;
}

void
fe_beep(void)
{

}

void
fe_exit(void)
{

}

void
fe_get_int (char *msg, int def, void *callback, void *userdata)
{

}

void
fe_get_str (char *msg, char *def, void *callback, void *userdata)
{

}

void
fe_open_url (const char *url)
{

}

void fe_ctrl_gui (session *sess, int action, int arg) {}
int fe_gui_info (session *sess, int info_type) { return 0; }

void *
fe_gui_info_ptr (session *sess, int info_type)
{
	return NULL;
}

void
fe_uselect (session *sess, char *word[], int do_clear, int scroll_to)
{

}

char *
strip_colour(char *string)
{
        char *c = string;
        char *c2 = string;
        char *last_non_space = NULL;
        /* c is source, c2 is target */
        for(; c && *c; c++)
                switch (*c)
                {
                case 3:
                        if(isdigit(c[1]))
                        {
                                c++;
                                if(isdigit(c[1]))
                                        c++;
                                if(c[1] == ',' && isdigit(c[2]))
                                {
                                        c += 2;
                                        if(isdigit(c[1]))
                                                c++;
                                }
                        }
                        break;
                case 2:
                case 6:
                case 7:
                case 22:
                case 23:
                case 27:
                case 31:
                        break;
                case 32:
                        *c2++ = *c;
                        break;
                default:
                        *c2++ = *c;
                        last_non_space = c2;
                        break;
                }
        *c2 = '\0';
        if(last_non_space)
                *last_non_space = '\0';
        return string;
}

void
fe_message (char *msg, int flags)
{
	printf("[%ld] %s\n", time(NULL), strip_colour(msg));
}

void
fe_confirm (const char *message, void (*yesproc)(void *), void (*noproc)(void *), void *ud)
{

}

void
fe_set_color_paste (session *sess, int status)
{

}

void
fe_serverlist_open (session *sess)
{

}

void
fe_userlist_insert (struct session *sess, struct User *newuser, int row, int sel)
{

}

int
fe_userlist_remove (struct session *sess, struct User *user)
{
	return 1;
}

void
fe_userlist_rehash (struct session *sess, struct User *user)
{

}

void
fe_userlist_move (struct session *sess, struct User *user, int new_row)
{

}

void
fe_new_window (struct session *sess, int focus)
{

}

void
fe_new_server (struct server *serv)
{

}

void
fe_set_lag(server *serv, int lag)
{

}

int
fe_is_banwindow (struct session *sess)
{
	return FALSE;
}

void
fe_dcc_add (struct DCC *dcc)
{

}

void
fe_dcc_update (struct DCC *dcc)
{

}

void
fe_dcc_remove (struct DCC *dcc)
{

}

int
fe_dcc_open_recv_win (int passive)
{
	return 1;
}

int
fe_dcc_open_send_win (int passive)
{
	return 1;
}

int
fe_dcc_open_chat_win (int passive)
{
	return 1;
}

void
fe_set_throttle (server *serv)
{

}

void
fe_set_away (server *serv)
{

}

void
fe_set_topic (struct session *sess, char *topic)
{
	printf("topic set: %s\n", topic);
}

void
fe_set_hilight (struct session *sess)
{

}

void
fe_set_tab_color (struct session *sess, int col)
{

}

void
fe_flash_window (struct session *sess)
{

}

void
fe_update_mode_buttons (struct session *sess, char mode, char sign)
{

}

void
fe_update_channel_key (struct session *sess)
{

}

void
fe_update_channel_limit (struct session *sess)
{

}

void fe_clear_channel (struct session *sess) {}
void fe_session_callback (struct session *sess) {}
void fe_server_callback (struct server *serv) {}
void fe_url_add (const char *text) {}
void fe_pluginlist_update (void) {}
void fe_buttons_update (struct session *sess) {}
void fe_dlgbuttons_update (struct session *sess) {}

char *fe_get_inputbox_contents (struct session *sess) { return NULL; }
int fe_get_inputbox_cursor (struct session *sess) { return 0; }
void fe_set_inputbox_contents (struct session *sess, char *text) {}
void fe_set_inputbox_cursor (struct session *sess, int delta, int pos) {}

void fe_tray_set_flash (const char *filename1, const char *filename2, int timeout) {}
void fe_tray_set_file (const char *filename) {}

void fe_tray_set_icon (feicon icon) {}
void fe_tray_set_tooltip (const char *text) {}
void fe_tray_set_balloon (const char *title, const char *text) {}

void fe_progressbar_start (struct session *sess) {}
void fe_progressbar_end (struct server *serv) {}

void fe_userlist_numbers (struct session *sess) {}
void fe_userlist_numbers_block (struct session *sess) {}
void fe_userlist_numbers_unblock (struct session *sess) {}
void fe_userlist_clear (struct session *sess) {}
void fe_userlist_set_selected (struct session *sess) {}

void fe_set_channel (struct session *sess) {}
void fe_set_title (struct session *sess) {}
void fe_set_nonchannel (struct session *sess, int state) {}
void fe_set_nick (struct server *serv, char *newnick) {}
void fe_ignore_update (int level) {}

void fe_menu_del (menu_entry *m) {}
char *fe_menu_add (menu_entry *m) { return NULL; }
void fe_menu_update (menu_entry *m) {}

void fe_add_ban_list (struct session *sess, char *mask, char *who, char *when, int is_exemption) {}
void fe_ban_list_end (struct session *sess, int is_exemption) {}
void fe_notify_update (char *name) {}

int fe_is_chanwindow (struct server *serv) { return 0; }
void fe_add_chan_list (struct server *serv, char *chan, char *users, char *topic) {}
void fe_chan_list_end (struct server *serv) {}

void fe_lastlog (session *sess, session *lastlog_sess, char *sstr, gboolean regexp) {}
void fe_close_window (struct session *sess) {}
void fe_server_event (server *serv, int type, int arg) {}

void
fe_print_text (struct session *sess, char *text, time_t stamp)
{
	printf("[%ld] %s", stamp, strip_colour(text));
}

void
fe_init (void)
{
}

void
fe_main (void)
{
	GMainLoop *loop = g_main_loop_new(NULL, TRUE);
	g_main_loop_run(loop);
}

void fe_cleanup (void) {}

void fe_get_file (const char *title, char *initial,
                  void (*callback) (void *userdata, char *file), void *userdata,
                  int flags)
{

}

int
fe_input_add (int sok, int flags, void *func, void *data)
{
        int tag, type = 0;
        GIOChannel *channel;

        channel = g_io_channel_unix_new (sok);

        if (flags & FIA_READ)
                type |= G_IO_IN | G_IO_HUP | G_IO_ERR;
        if (flags & FIA_WRITE)
                type |= G_IO_OUT | G_IO_ERR;
        if (flags & FIA_EX)
                type |= G_IO_PRI;

        tag = g_io_add_watch (channel, type, (GIOFunc) func, data);
        g_io_channel_unref (channel);

        return tag;
}

void fe_text_clear(session *sess) {}
void fe_dcc_send_filereq (struct session *sess, char *nick, int maxcps, int passive) {}

void
fe_add_rawlog (struct server *serv, char *text, int len, int outbound)
{
#ifdef DISPLAY_RAWLOG
	printf("[%s] %s %s", serv->name, outbound ? "<<" : ">>", text);
#endif
}

