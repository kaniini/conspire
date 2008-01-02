/* Conspire
 * Copyright (C) 2008 William Pitcock
 *
 * XChat
 * Copyright (C) 1998 Peter Zelezny.
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

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "xchat.h"
#include "cfgfiles.h"
#include "util.h"
#include "fe.h"
#include "text.h"
#include "xchatc.h"

#define XCHAT_DIR ".conspire"
#define DEF_FONT "Sans 9"

void
list_addentry (GSList ** list, char *cmd, char *name)
{
	struct popup *pop;
	int cmd_len = 1, name_len;

	/* remove <2.8.0 stuff */
	if (!strcmp (cmd, "away") && !strcmp (name, "BACK"))
		return;

	if (cmd)
		cmd_len = strlen (cmd) + 1;
	name_len = strlen (name) + 1;

	pop = malloc (sizeof (struct popup) + cmd_len + name_len);
	pop->name = (char *) pop + sizeof (struct popup);
	pop->cmd = pop->name + name_len;

	memcpy (pop->name, name, name_len);
	if (cmd)
		memcpy (pop->cmd, cmd, cmd_len);
	else
		pop->cmd[0] = 0;

	*list = g_slist_append (*list, pop);
}

/* read it in from a buffer to our linked list */

static void
list_load_from_data (GSList ** list, char *ibuf, int size)
{
	char cmd[384];
	char name[128];
	char *buf;
	int pnt = 0;

	cmd[0] = 0;
	name[0] = 0;

	while (buf_get_line (ibuf, &buf, &pnt, size))
	{
		if (*buf != '#')
		{
			if (!strncasecmp (buf, "NAME ", 5))
			{
				g_strlcpy (name, buf + 5, sizeof (name));
			}
			else if (!strncasecmp (buf, "CMD ", 4))
			{
				g_strlcpy (cmd, buf + 4, sizeof (cmd));
				if (*name)
				{
					list_addentry (list, cmd, name);
					cmd[0] = 0;
					name[0] = 0;
				}
			}
		}
	}
}

void
list_loadconf (char *file, GSList ** list, char *defaultconf)
{
	char filebuf[256];
	char *ibuf;
	int fh;
	struct stat st;

	snprintf (filebuf, sizeof (filebuf), "%s/%s", get_xdir_fs (), file);
	fh = open (filebuf, O_RDONLY | OFLAGS);
	if (fh == -1)
	{
		if (defaultconf)
			list_load_from_data (list, defaultconf, strlen (defaultconf));
		return;
	}
	if (fstat (fh, &st) != 0)
	{
		perror ("fstat");
		abort ();
	}

	ibuf = malloc (st.st_size);
	read (fh, ibuf, st.st_size);
	close (fh);

	list_load_from_data (list, ibuf, st.st_size);

	free (ibuf);
}

void
list_free (GSList ** list)
{
	void *data;
	while (*list)
	{
		data = (void *) (*list)->data;
		free (data);
		*list = g_slist_remove (*list, data);
	}
}

int
list_delentry (GSList ** list, char *name)
{
	struct popup *pop;
	GSList *alist = *list;

	while (alist)
	{
		pop = (struct popup *) alist->data;
		if (!strcasecmp (name, pop->name))
		{
			*list = g_slist_remove (*list, pop);
			free (pop);
			return 1;
		}
		alist = alist->next;
	}
	return 0;
}

char *
cfg_get_str (char *cfg, const char *var, char **dest)
{
	while (1)
	{
		if (!strncasecmp (var, cfg, strlen (var)))
		{
			char *value, t;
			cfg += strlen (var);
			while (*cfg == ' ')
				cfg++;
			if (*cfg == '=')
				cfg++;
			while (*cfg == ' ')
				cfg++;
			/*while (*cfg == ' ' || *cfg == '=')
			   cfg++; */
			value = cfg;
			while (*cfg != 0 && *cfg != '\n')
				cfg++;
			t = *cfg;
			*cfg = 0;
			*dest = g_strdup(value);
			*cfg = t;
			return cfg;
		}
		while (*cfg != 0 && *cfg != '\n')
			cfg++;
		if (*cfg == 0)
			return 0;
		cfg++;
		if (*cfg == 0)
			return 0;
	}
}

static int
cfg_put_str (int fh, const char *var, const char *value)
{
	char buf[512];
	int len;

	snprintf (buf, sizeof buf, "%s = %s\n", var, value ? value : "");

	len = strlen (buf);
	return (write (fh, buf, len) == len);
}

int
cfg_put_color (int fh, int r, int g, int b, char *var)
{
	char buf[400];
	int len;

	snprintf (buf, sizeof buf, "%s = %04x %04x %04x\n", var, r, g, b);
	len = strlen (buf);
	return (write (fh, buf, len) == len);
}

int
cfg_put_int (int fh, int value, const char *var)
{
	char buf[400];
	int len;

	if (value == -1)
		value = 1;

	snprintf (buf, sizeof buf, "%s = %d\n", var, value);
	len = strlen (buf);
	return (write (fh, buf, len) == len);
}

int
cfg_get_color (char *cfg, char *var, int *r, int *g, int *b)
{
	char *str;

	if (!cfg_get_str (cfg, var, &str))
		return 0;

	sscanf (str, "%04x %04x %04x", r, g, b);
	g_free(str);

	return 1;
}

int
cfg_get_int_with_result (char *cfg, const char *var, int *result)
{
	char *str;
	int ret;

	if (!cfg_get_str (cfg, var, &str))
	{
		*result = 0;
		return 0;
	}

	*result = 1;
	ret = atoi(str);
	g_free(str);

	return ret;
}

int
cfg_get_int (char *cfg, const char *var)
{
	char *str;
	int ret;

	if (!cfg_get_str (cfg, var, &str))
		return 0;

	ret = atoi(str);
	g_free(str);

	return ret;
}

char *xdir_fs = NULL;	/* file system encoding */
char *xdir_utf = NULL;	/* utf-8 encoding */

char *
get_xdir_fs (void)
{
	if (!xdir_fs)
		xdir_fs = g_strdup_printf ("%s/" XCHAT_DIR, g_get_home_dir ());

	return xdir_fs;
}

char *
get_xdir_utf8 (void)
{
	if (!xdir_utf)	/* never free this, keep it for program life time */
		xdir_utf = xchat_filename_to_utf8 (get_xdir_fs (), -1, 0, 0, 0);

	return xdir_utf;
}

static void
check_prefs_dir (void)
{
	char *dir = get_xdir_fs ();
	if (access (dir, F_OK) != 0)
	{
		if (mkdir (dir, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
			fe_message (_("Cannot create ~/.xchat2"), FE_MSG_ERROR);
	}
}

static char *
default_file (void)
{
	static char *dfile = 0;

	if (!dfile)
	{
		dfile = malloc (strlen (get_xdir_fs ()) + 12);
		sprintf (dfile, "%s/xchat.conf", get_xdir_fs ());
	}
	return dfile;
}

/* Keep these sorted!! */

PrefsEntry vars[] = {
	{"auto_save", PREFS_TYPE_BOOL, &prefs.autosave},
	{"auto_save_url", PREFS_TYPE_BOOL, &prefs.autosave_url},

	{"away_auto_unmark", PREFS_TYPE_BOOL, &prefs.auto_unmark_away},
	{"away_reason", PREFS_TYPE_STR, &prefs.awayreason},
	{"away_show_message", PREFS_TYPE_BOOL, &prefs.show_away_message},
	{"away_show_once", PREFS_TYPE_BOOL, &prefs.show_away_once},
	{"away_size_max", PREFS_TYPE_INT, &prefs.away_size_max},
	{"away_timeout", PREFS_TYPE_INT, &prefs.away_timeout},
	{"away_track", PREFS_TYPE_BOOL, &prefs.away_track},

	{"completion_amount", PREFS_TYPE_INT, &prefs.completion_amount},
	{"completion_auto", PREFS_TYPE_BOOL, &prefs.nickcompletion},
	{"completion_sort", PREFS_TYPE_INT, &prefs.completion_sort},
	{"completion_suffix", PREFS_TYPE_STR, &prefs.nick_suffix},

	{"dcc_auto_chat", PREFS_TYPE_INT, &prefs.autodccchat},
	{"dcc_auto_resume", PREFS_TYPE_BOOL, &prefs.autoresume},
	{"dcc_auto_send", PREFS_TYPE_INT, &prefs.autodccsend},
	{"dcc_blocksize", PREFS_TYPE_INT, &prefs.dcc_blocksize},
	{"dcc_completed_dir", PREFS_TYPE_STR, &prefs.dcc_completed_dir},
	{"dcc_dir", PREFS_TYPE_STR, &prefs.dccdir},
	{"dcc_fast_send", PREFS_TYPE_BOOL, &prefs.fastdccsend},
	{"dcc_global_max_get_cps", PREFS_TYPE_INT, &prefs.dcc_global_max_get_cps},
	{"dcc_global_max_send_cps", PREFS_TYPE_INT, &prefs.dcc_global_max_send_cps},
	{"dcc_ip", PREFS_TYPE_STR, &prefs.dcc_ip_str},
	{"dcc_ip_from_server", PREFS_TYPE_BOOL, &prefs.ip_from_server},
	{"dcc_max_get_cps", PREFS_TYPE_INT, &prefs.dcc_max_get_cps},
	{"dcc_max_send_cps", PREFS_TYPE_INT, &prefs.dcc_max_send_cps},
	{"dcc_permissions", PREFS_TYPE_INT, &prefs.dccpermissions},
	{"dcc_port_first", PREFS_TYPE_INT, &prefs.first_dcc_send_port},
	{"dcc_port_last", PREFS_TYPE_INT, &prefs.last_dcc_send_port},
	{"dcc_remove", PREFS_TYPE_BOOL, &prefs.dcc_remove},
	{"dcc_save_nick", PREFS_TYPE_BOOL, &prefs.dccwithnick},
	{"dcc_send_fillspaces", PREFS_TYPE_BOOL, &prefs.dcc_send_fillspaces},
	{"dcc_stall_timeout", PREFS_TYPE_INT, &prefs.dccstalltimeout},
	{"dcc_timeout", PREFS_TYPE_INT, &prefs.dcctimeout},

	{"dnsprogram", PREFS_TYPE_STR, &prefs.dnsprogram},

	{"flood_ctcp_num", PREFS_TYPE_INT, &prefs.ctcp_number_limit},
	{"flood_ctcp_time", PREFS_TYPE_INT, &prefs.ctcp_time_limit},
	{"flood_msg_num", PREFS_TYPE_INT, &prefs.msg_number_limit},
	{"flood_msg_time", PREFS_TYPE_INT, &prefs.msg_time_limit},

	{"gui_auto_open_chat", PREFS_TYPE_BOOL, &prefs.autoopendccchatwindow},
	{"gui_auto_open_dialog", PREFS_TYPE_BOOL, &prefs.autodialog},
	{"gui_auto_open_recv", PREFS_TYPE_BOOL, &prefs.autoopendccrecvwindow},
	{"gui_auto_open_send", PREFS_TYPE_BOOL, &prefs.autoopendccsendwindow},
	{"gui_colors_from_gtk", PREFS_TYPE_BOOL, &prefs.gtk_colors},
	{"gui_dialog_height", PREFS_TYPE_INT, &prefs.dialog_height},
	{"gui_dialog_left", PREFS_TYPE_INT, &prefs.dialog_left},
	{"gui_dialog_top", PREFS_TYPE_INT, &prefs.dialog_top},
	{"gui_dialog_width", PREFS_TYPE_INT, &prefs.dialog_width},
	{"gui_hide_menu", PREFS_TYPE_BOOL, &prefs.hidemenu},
	{"gui_input_spell", PREFS_TYPE_BOOL, &prefs.gui_input_spell},
	{"gui_input_style", PREFS_TYPE_BOOL, &prefs.style_inputbox},
	{"gui_join_dialog", PREFS_TYPE_BOOL, &prefs.gui_join_dialog},
	{"gui_lagometer", PREFS_TYPE_INT, &prefs.lagometer},
	{"gui_mode_buttons", PREFS_TYPE_BOOL, &prefs.chanmodebuttons},
	{"gui_pane_left_size", PREFS_TYPE_INT, &prefs.gui_pane_left_size},
	{"gui_pane_right_size", PREFS_TYPE_INT, &prefs.gui_pane_right_size},
	{"gui_quit_dialog", PREFS_TYPE_BOOL, &prefs.gui_quit_dialog},
	{"gui_slist_select", PREFS_TYPE_INT, &prefs.slist_select},
	{"gui_slist_skio", PREFS_TYPE_BOOL, &prefs.skip_serverlist},
	{"gui_throttlemeter", PREFS_TYPE_INT, &prefs.throttlemeter},
	{"gui_topicbar", PREFS_TYPE_BOOL, &prefs.topicbar},
	{"gui_tray", PREFS_TYPE_BOOL, &prefs.gui_tray},
	{"gui_tray_flags", PREFS_TYPE_INT, &prefs.gui_tray_flags},
	{"gui_tweaks", PREFS_TYPE_INT, &prefs.gui_tweaks},
	{"gui_ulist_buttons", PREFS_TYPE_BOOL, &prefs.userlistbuttons},
	{"gui_ulist_doubleclick", PREFS_TYPE_STR, &prefs.doubleclickuser},
	{"gui_ulist_hide", PREFS_TYPE_BOOL, &prefs.hideuserlist},
	{"gui_ulist_left", PREFS_TYPE_BOOL, &prefs._gui_ulist_left},	/* obsolete */
	{"gui_ulist_pos", PREFS_TYPE_INT, &prefs.gui_ulist_pos},
	{"gui_ulist_resizable", PREFS_TYPE_BOOL, &prefs.paned_userlist},
	{"gui_ulist_show_hosts", PREFS_TYPE_BOOL, &prefs.showhostname_in_userlist},
	{"gui_ulist_sort", PREFS_TYPE_INT, &prefs.userlist_sort},
	{"gui_ulist_style", PREFS_TYPE_BOOL, &prefs.style_namelistgad},
	{"gui_url_mod", PREFS_TYPE_INT, &prefs.gui_url_mod},
	{"gui_usermenu", PREFS_TYPE_BOOL, &prefs.gui_usermenu},
	{"gui_win_height", PREFS_TYPE_INT, &prefs.mainwindow_height},
	{"gui_win_left", PREFS_TYPE_INT, &prefs.mainwindow_left},
	{"gui_win_save", PREFS_TYPE_BOOL, &prefs.mainwindow_save},
	{"gui_win_state", PREFS_TYPE_INT, &prefs.gui_win_state},
	{"gui_win_top", PREFS_TYPE_INT, &prefs.mainwindow_top},
	{"gui_win_width", PREFS_TYPE_INT, &prefs.mainwindow_width},

	{"input_balloon_chans", PREFS_TYPE_BOOL, &prefs.input_balloon_chans},
	{"input_balloon_hilight", PREFS_TYPE_BOOL, &prefs.input_balloon_hilight},
	{"input_balloon_priv", PREFS_TYPE_BOOL, &prefs.input_balloon_priv},
	{"input_beep_chans", PREFS_TYPE_BOOL, &prefs.input_beep_chans},
	{"input_beep_hilight", PREFS_TYPE_BOOL, &prefs.input_beep_hilight},
	{"input_beep_msg", PREFS_TYPE_BOOL, &prefs.input_beep_priv},
	{"input_command_char", PREFS_TYPE_STR, &prefs.cmdchar},
	{"input_filter_beep", PREFS_TYPE_BOOL, &prefs.filterbeep},
	{"input_flash_chans", PREFS_TYPE_BOOL, &prefs.input_flash_chans},
	{"input_flash_hilight", PREFS_TYPE_BOOL, &prefs.input_flash_hilight},
	{"input_flash_priv", PREFS_TYPE_BOOL, &prefs.input_flash_priv},
	{"input_perc_ascii", PREFS_TYPE_BOOL, &prefs.perc_ascii},
	{"input_perc_color", PREFS_TYPE_BOOL, &prefs.perc_color},
	{"input_tray_chans", PREFS_TYPE_BOOL, &prefs.input_tray_chans},
	{"input_tray_hilight", PREFS_TYPE_BOOL, &prefs.input_tray_hilight},
	{"input_tray_priv", PREFS_TYPE_BOOL, &prefs.input_tray_priv},

	{"irc_auto_rejoin", PREFS_TYPE_BOOL, &prefs.autorejoin},
	{"irc_ban_type", PREFS_TYPE_INT, &prefs.bantype},
	{"irc_conf_mode", PREFS_TYPE_BOOL, &prefs.confmode},
	{"irc_extra_hilight", PREFS_TYPE_STR, &prefs.irc_extra_hilight},
	{"irc_hide_version", PREFS_TYPE_BOOL, &prefs.hidever},
	{"irc_id_ntext", PREFS_TYPE_STR, &prefs.irc_id_ntext},
	{"irc_id_ytext", PREFS_TYPE_STR, &prefs.irc_id_ytext},
	{"irc_invisible", PREFS_TYPE_BOOL, &prefs.invisible},
	{"irc_join_delay", PREFS_TYPE_INT, &prefs.irc_join_delay},
	{"irc_logging", PREFS_TYPE_BOOL, &prefs.logging},
	{"irc_logmask", PREFS_TYPE_STR, &prefs.logmask},
	{"irc_nick1", PREFS_TYPE_STR, &prefs.nick1},
	{"irc_nick2", PREFS_TYPE_STR, &prefs.nick2},
	{"irc_nick3", PREFS_TYPE_STR, &prefs.nick3},
	{"irc_nick_hilight", PREFS_TYPE_STR, &prefs.irc_nick_hilight},
	{"irc_no_hilight", PREFS_TYPE_STR, &prefs.irc_no_hilight},
	{"irc_part_reason", PREFS_TYPE_STR, &prefs.partreason},
	{"irc_quit_reason", PREFS_TYPE_STR, &prefs.quitreason},
	{"irc_real_name", PREFS_TYPE_STR, &prefs.realname},
	{"irc_servernotice", PREFS_TYPE_BOOL, &prefs.servernotice},
	{"irc_skip_motd", PREFS_TYPE_BOOL, &prefs.skipmotd},
	{"irc_user_name", PREFS_TYPE_STR, &prefs.username},
	{"irc_wallops", PREFS_TYPE_BOOL, &prefs.wallops},
	{"irc_who_join", PREFS_TYPE_BOOL, &prefs.userhost},

	{"net_auto_reconnect", PREFS_TYPE_BOOL, &prefs.autoreconnect},
	{"net_auto_reconnectonfail", PREFS_TYPE_BOOL, &prefs.autoreconnectonfail},
	{"net_bind_host", PREFS_TYPE_STR, &prefs.hostname},
	{"net_ping_timeout", PREFS_TYPE_INT, &prefs.pingtimeout},
	{"net_proxy_auth", PREFS_TYPE_BOOL, &prefs.proxy_auth},
	{"net_proxy_host", PREFS_TYPE_STR, &prefs.proxy_host},
	{"net_proxy_pass", PREFS_TYPE_STR, &prefs.proxy_pass},
	{"net_proxy_port", PREFS_TYPE_INT, &prefs.proxy_port},
	{"net_proxy_type", PREFS_TYPE_INT, &prefs.proxy_type},
	{"net_proxy_use", PREFS_TYPE_INT, &prefs.proxy_use},
	{"net_proxy_user", PREFS_TYPE_STR, &prefs.proxy_user},

	{"net_reconnect_delay", PREFS_TYPE_INT, &prefs.recon_delay},
	{"net_throttle", PREFS_TYPE_BOOL, &prefs.throttle},

	{"notify_timeout", PREFS_TYPE_INT, &prefs.notify_timeout},
	{"notify_whois_online", PREFS_TYPE_BOOL, &prefs.whois_on_notifyonline},

	{"perl_warnings", PREFS_TYPE_BOOL, &prefs.perlwarnings},

	{"redundant_nickstamps", PREFS_TYPE_BOOL, &prefs.redundant_nickstamps},

	{"sound_command", PREFS_TYPE_STR, &prefs.soundcmd},
	{"sound_dir", PREFS_TYPE_STR, &prefs.sounddir},
	{"stamp_log", PREFS_TYPE_BOOL, &prefs.timestamp_logs},
	{"stamp_log_format", PREFS_TYPE_STR, &prefs.timestamp_log_format},
	{"stamp_text", PREFS_TYPE_BOOL, &prefs.timestamp},
	{"stamp_text_format", PREFS_TYPE_STR, &prefs.stamp_format},

	{"strip_quits", PREFS_TYPE_BOOL, &prefs.strip_quits},

	{"tab_chans", PREFS_TYPE_BOOL, &prefs.tabchannels},
	{"tab_dialogs", PREFS_TYPE_BOOL, &prefs.privmsgtab},
	{"tab_icons", PREFS_TYPE_BOOL, &prefs.tab_icons},
	{"tab_layout", PREFS_TYPE_INT, &prefs.tab_layout},
	{"tab_new_to_front", PREFS_TYPE_INT, &prefs.newtabstofront},
	{"tab_notices", PREFS_TYPE_BOOL, &prefs.notices_tabs},
	{"tab_pos", PREFS_TYPE_INT, &prefs.tab_pos},
	{"tab_position", PREFS_TYPE_INT, &prefs._tabs_position}, /* obsolete */
	{"tab_server", PREFS_TYPE_BOOL, &prefs.use_server_tab},
	{"tab_small", PREFS_TYPE_INT, &prefs.tab_small},
	{"tab_sort", PREFS_TYPE_BOOL, &prefs.tab_sort},
	{"tab_trunc", PREFS_TYPE_INT, &prefs.truncchans},
	{"tab_utils", PREFS_TYPE_BOOL, &prefs.windows_as_tabs},

	{"text_background", PREFS_TYPE_STR, &prefs.background},
	{"text_color_nicks", PREFS_TYPE_BOOL, &prefs.colorednicks},
	{"text_color_nicks_hilighted", PREFS_TYPE_BOOL, &prefs.coloredhnicks},
	{"text_font", PREFS_TYPE_STR, &prefs.font_normal},
	{"text_indent", PREFS_TYPE_BOOL, &prefs.indent_nicks},
	{"text_max_indent", PREFS_TYPE_INT, &prefs.max_auto_indent},
	{"text_max_lines", PREFS_TYPE_INT, &prefs.max_lines},
	{"text_replay", PREFS_TYPE_BOOL, &prefs.text_replay},
	{"text_show_marker", PREFS_TYPE_BOOL, &prefs.show_marker},
	{"text_show_sep", PREFS_TYPE_BOOL, &prefs.show_separator},
	{"text_stripcolor", PREFS_TYPE_BOOL, &prefs.stripcolor},
	{"text_thin_sep", PREFS_TYPE_BOOL, &prefs.thin_separator},
	{"text_tint_blue", PREFS_TYPE_INT, &prefs.tint_blue},
	{"text_tint_green", PREFS_TYPE_INT, &prefs.tint_green},
	{"text_tint_red", PREFS_TYPE_INT, &prefs.tint_red},
	{"text_transparent", PREFS_TYPE_BOOL, &prefs.transparent},
	{"text_wordwrap", PREFS_TYPE_BOOL, &prefs.wordwrap},

	{0, 0, 0},
};

static char *
convert_with_fallback (const char *str, const char *fallback)
{
	char *utf;

	utf = g_locale_to_utf8 (str, -1, 0, 0, 0);
	if (!utf)
	{
		/* this can happen if CHARSET envvar is set wrong */
		/* maybe it's already utf8 (breakage!) */
		if (!g_utf8_validate (str, -1, NULL))
			utf = g_strdup (fallback);
		else
			utf = g_strdup (str);
	}

	return utf;
}

void
load_config (void)
{
	struct stat st;
	char *cfg, *sp;
	const char *username, *realname;
	int res, val, i, fh;

	check_prefs_dir ();
	username = g_get_user_name ();
	if (!username)
		username = "root";

	realname = g_get_real_name ();
	if ((realname && realname[0] == 0) || !realname)
		realname = username;

	username = convert_with_fallback (username, "username");
	realname = convert_with_fallback (realname, "realname");

	memset (&prefs, 0, sizeof (struct xchatprefs));

	/* put in default values, anything left out is automatically zero */
	prefs.local_ip = 0xffffffff;
	prefs.redundant_nickstamps = TRUE;
	prefs.strip_quits = TRUE;
	prefs.irc_join_delay = 3;
	prefs.show_marker = 1;
	prefs.newtabstofront = 2;
	prefs.completion_amount = 5;
	prefs.away_timeout = 60;
	prefs.away_size_max = 300;
	prefs.away_track = 1;
	prefs.timestamp_logs = 1;
	prefs.truncchans = 20;
	prefs.autoresume = 1;
	prefs.show_away_once = 1;
	prefs.indent_nicks = 1;
	prefs.thin_separator = 1;
	prefs._tabs_position = 2; /* 2 = left */
	prefs.fastdccsend = 1;
	prefs.wordwrap = 1;
	prefs.autosave = 1;
	prefs.autodialog = 1;
	prefs.gtk_colors = 1;
	prefs.gui_input_spell = 1;
	prefs.autoreconnect = 1;
	prefs.recon_delay = 10;
	prefs.text_replay = 1;
	prefs.tabchannels = 1;
	prefs.tab_layout = 2;	/* 0=Tabs 1=Reserved 2=Tree */
	prefs.tab_sort = 1;
	prefs.paned_userlist = 1;
	prefs.newtabstofront = 2;
	prefs.use_server_tab = 1;
	prefs.privmsgtab = 1;
	/*prefs.style_inputbox = 1;*/
	prefs.dccpermissions = 0600;
	prefs.max_lines = 500;
	prefs.mainwindow_width = 640;
	prefs.mainwindow_height = 400;
	prefs.dialog_width = 500;
	prefs.dialog_height = 256;
	prefs.gui_join_dialog = 1;
	prefs.gui_quit_dialog = 1;
	prefs.dcctimeout = 180;
	prefs.dccstalltimeout = 60;
	prefs.notify_timeout = 15;
	prefs.tint_red =
		prefs.tint_green =
		prefs.tint_blue = 195;
	prefs.auto_indent = 1;
	prefs.max_auto_indent = 256;
	prefs.show_separator = 1;
	prefs.dcc_blocksize = 1024;
	prefs.throttle = 1;
	 /*FIXME*/ prefs.msg_time_limit = 30;
	prefs.msg_number_limit = 5;
	prefs.ctcp_time_limit = 30;
	prefs.ctcp_number_limit = 5;
	prefs.topicbar = 1;
	prefs.lagometer = 1;
	prefs.throttlemeter = 1;
	prefs.autoopendccrecvwindow = 1;
	prefs.autoopendccsendwindow = 1;
	prefs.autoopendccchatwindow = 1;
	prefs.userhost = 1;
	prefs.gui_url_mod = 4;	/* ctrl */
	prefs.gui_tray = 1;
	prefs.gui_pane_left_size = 100;
	prefs.gui_pane_right_size = 100;
	prefs.mainwindow_save = 1;
	prefs.bantype = 2;
	prefs.input_flash_priv = prefs.input_flash_hilight = 1;
	prefs.input_tray_priv = prefs.input_tray_hilight = 1;
	prefs.autodccsend = 2;	/* browse mode */
	prefs.stamp_format = strdup("[%H:%M] ");
	prefs.timestamp_log_format = strdup("%b %d %H:%M:%S ");
	prefs.logmask = strdup("%n-%c.log");
	prefs.nick_suffix = strdup(",");
	prefs.cmdchar = strdup("/");
	prefs.nick1 = strdup(username);
	prefs.nick2 = g_strdup_printf("%s_", username);
	prefs.nick3 = g_strdup_printf("%s__", username);
	prefs.realname = strdup(realname);
	prefs.username = strdup(username);
	prefs.sounddir = g_strdup_printf("%s/sounds", get_xdir_utf8());
	prefs.dccdir = g_strdup_printf("%s/downloads", get_xdir_utf8());
	prefs.doubleclickuser = strdup("QUOTE WHOIS %s %s");
	prefs.awayreason = strdup(_("I'm busy"));
	prefs.quitreason = strdup(_("Leaving"));
	prefs.partreason = strdup(prefs.quitreason);
	prefs.font_normal = strdup(DEF_FONT);
	prefs.dnsprogram = strdup("host");
	prefs.irc_no_hilight = strdup("NickServ,ChanServ");

	g_free ((char *)username);
	g_free ((char *)realname);

	fh = open (default_file (), OFLAGS | O_RDONLY);
	if (fh != -1)
	{
		fstat (fh, &st);
		cfg = malloc (st.st_size + 1);
		cfg[0] = '\0';
		i = read (fh, cfg, st.st_size);
		if (i >= 0)
			cfg[i] = '\0';					/* make sure cfg is NULL terminated */
		close (fh);
		i = 0;
		do
		{
			switch (vars[i].type)
			{
			case PREFS_TYPE_STR:
				cfg_get_str (cfg, vars[i].name, vars[i].ptr);
				break;
			case PREFS_TYPE_BOOL:
			case PREFS_TYPE_INT:
				val = cfg_get_int_with_result (cfg, vars[i].name, &res);
				if (res)
					*((int *) vars[i].ptr) = val;
				break;
			}
			i++;
		}
		while (vars[i].name);

		free (cfg);

	} else
	{
		mkdir_utf8 (prefs.dccdir);
		mkdir_utf8 (prefs.dcc_completed_dir);
	}
	if (prefs.mainwindow_height < 138)
		prefs.mainwindow_height = 138;
	if (prefs.mainwindow_width < 106)
		prefs.mainwindow_width = 106;

	sp = strchr (prefs.username, ' ');
	if (sp)
		sp[0] = 0;	/* spaces in username would break the login */

	/* try to make sense of old ulist/tree position settings */
	if (prefs.gui_ulist_pos == 0)
	{
		prefs.gui_ulist_pos = 3;	/* top right */
		if (prefs._gui_ulist_left)
			prefs.gui_ulist_pos = 2;	/* bottom left */

		switch (prefs._tabs_position)
		{
		case 0:
			prefs.tab_pos = 6; /* bottom */
			break;
		case 1:
			prefs.tab_pos = 5;	/* top */
			break;
		case 2:
			prefs.tab_pos = 1; 	/* left */
			break;
		case 3:
			prefs.tab_pos = 4; 	/* right */
			break;
		case 4:
			prefs.tab_pos = 1;	/* (hidden)left */
			break;
		case 5:
			if (prefs._gui_ulist_left)
			{
				prefs.tab_pos = 1; 	/* above ulist left */
				prefs.gui_ulist_pos = 2;
			}
			else
			{
				prefs.tab_pos = 3; 	/* above ulist right */
				prefs.gui_ulist_pos = 4;
			}
			break;
		}
	}
}

int
save_config (void)
{
	int fh, i;
	char *new_config, *config;

	check_prefs_dir ();

	config = default_file ();
	new_config = malloc (strlen (config) + 5);
	strcpy (new_config, config);
	strcat (new_config, ".new");
	
	fh = open (new_config, OFLAGS | O_TRUNC | O_WRONLY | O_CREAT, 0600);
	if (fh == -1)
	{
		free (new_config);
		return 0;
	}

	if (!cfg_put_str (fh, "version", PACKAGE_VERSION))
	{
		free (new_config);
		return 0;
	}
		
	i = 0;
	do
	{
		switch (vars[i].type)
		{
		case PREFS_TYPE_STR:
			if (*((char **) vars[i].ptr) == NULL)
			{
				i++;
				continue;
			}

			if (!cfg_put_str (fh, vars[i].name, *((char **) vars[i].ptr)))
			{
				free (new_config);
				return 0;
			}
			break;
		case PREFS_TYPE_INT:
		case PREFS_TYPE_BOOL:
			if (!cfg_put_int (fh, *((int *) vars[i].ptr), vars[i].name))
			{
				free (new_config);
				return 0;
			}
		}
		i++;
	}
	while (vars[i].name);

	if (close (fh) == -1)
	{
		free (new_config);
		return 0;
	}

	if (rename (new_config, config) == -1)
	{
		free (new_config);
		return 0;
	}
	free (new_config);

	return 1;
}

static void
set_showval (session *sess, const PrefsEntry *var, char *tbuf)
{
	int len, dots, j;
	static const char *offon[] = { "OFF", "ON" };

	len = strlen (var->name);
	memcpy (tbuf, var->name, len);
	dots = 29 - len;
	if (dots < 0)
		dots = 0;
	tbuf[len++] = '\003';
	tbuf[len++] = '2';
	for (j=0;j<dots;j++)
		tbuf[j + len] = '.';
	len += j;
	switch (var->type)
	{
	case PREFS_TYPE_STR:
		sprintf (tbuf + len, "\0033:\017 %s\n",
					*((char **) var->ptr) ? *((char **) var->ptr) : "");
		break;
	case PREFS_TYPE_INT:
		sprintf (tbuf + len, "\0033:\017 %d\n",
					*((int *) var->ptr));
		break;
	case PREFS_TYPE_BOOL:
		sprintf (tbuf + len, "\0033:\017 %s\n", offon[*((int *) var->ptr)]);
		break;
	}
	PrintText (sess, tbuf);
}

static void
set_list (session * sess, char *tbuf)
{
	int i;

	i = 0;
	do
	{
		set_showval (sess, &vars[i], tbuf);
		i++;
	}
	while (vars[i].name);
}

int
cfg_get_bool (char *var)
{
	int i = 0;

	do
	{
		if (!strcasecmp (var, vars[i].name))
		{
			return *((int *) vars[i].ptr);
		}
		i++;
	}
	while (vars[i].name);

	return -1;
}

int
cmd_set (struct session *sess, char *tbuf, char *word[], char *word_eol[])
{
	int wild = FALSE;
	int or = FALSE;
	int quiet = FALSE;
	int erase = FALSE;
	int i = 0, finds = 0, found;
	int idx = 2;
	char *var, *val;

	if (strcasecmp (word[2], "-e") == 0)
	{
		idx++;
		erase = TRUE;
	}

	if (strcasecmp (word[idx], "-or") == 0)
	{
		idx++;
		or = TRUE;
	}

	if (strcasecmp (word[idx], "-quiet") == 0)
	{
		idx++;
		quiet = TRUE;
	}

	var = word[idx];
	val = word_eol[idx+1];

	if (!*var)
	{
		set_list (sess, tbuf);
		return TRUE;
	}

	if ((strchr (var, '*') || strchr (var, '?')) && !*val)
		wild = TRUE;

	if (*val == '=')
		val++;

	do
	{
		if (wild)
			found = !match (var, vars[i].name);
		else
			found = strcasecmp (var, vars[i].name);

		if (found == 0)
		{
			finds++;
			switch (vars[i].type)
			{
			case PREFS_TYPE_STR:
				if (erase || *val)
				{
					g_free(vars[i].ptr);
					vars[i].ptr = g_strdup(val);
					if (!quiet)
						PrintTextf (sess, "%s set to: %s\n", var, *((char **) vars[i].ptr));
				} else
				{
					set_showval (sess, &vars[i], tbuf);
				}
				break;
			case PREFS_TYPE_INT:
			case PREFS_TYPE_BOOL:
				if (*val)
				{
					if (vars[i].type == PREFS_TYPE_BOOL)
					{
						if (atoi (val))
							*((int *) vars[i].ptr) = 1;
						else
							*((int *) vars[i].ptr) = 0;
						if (!strcasecmp (val, "YES") || !strcasecmp (val, "ON"))
							*((int *) vars[i].ptr) = 1;
						if (!strcasecmp (val, "NO") || !strcasecmp (val, "OFF"))
							*((int *) vars[i].ptr) = 0;
					} else
					{
						if (or) {
							int tmp = *((int *) vars[i].ptr);
							tmp |= atoi (val);
							*((int *) vars[i].ptr) = tmp;
						} else
							*((int *) vars[i].ptr) = atoi(val);
					}

					if (!quiet)
						PrintTextf (sess, "%s set to: %d\n", var, *((int *) vars[i].ptr));
				} else
				{
					set_showval (sess, &vars[i], tbuf);
				}
				break;
			}
		}
		i++;
	}
	while (vars[i].name);

	if (!finds && !quiet)
		PrintText (sess, "No such variable.\n");

	return TRUE;
}

int
xchat_open_file (char *file, int flags, int mode, int xof_flags)
{
	char buf[1024];

	if (xof_flags & XOF_FULLPATH)
		return open (file, flags | OFLAGS);

	snprintf (buf, sizeof (buf), "%s/%s", get_xdir_fs (), file);
	if (xof_flags & XOF_DOMODE)
		return open (buf, flags | OFLAGS, mode);
	else
		return open (buf, flags | OFLAGS);
}

FILE *
xchat_fopen_file (const char *file, const char *mode, int xof_flags)
{
	char buf[1024];

	if (xof_flags & XOF_FULLPATH)
		return fopen (file, mode);

	snprintf (buf, sizeof (buf), "%s/%s", get_xdir_fs (), file);
	return fopen (buf, mode);
}
