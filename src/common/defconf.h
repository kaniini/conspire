/* Conspire
 * Copyright (c) 2007 Atheme Development Team.
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

#ifndef DEFCONF_H
#define DEFCONF_H

#include <glib.h>
#include "configdb.h"

enum {
	CONF_STR,
	CONF_BOOL,
	CONF_INT,
	CONF_FLOAT,
	CONF_DOUBLE
};

union _Value {
	gchar *strv;
	gboolean boolv;
	gint intv;
	gfloat floatv;
	gdouble doublev;
};

struct Config {
	gchar *section;
	gchar *key;
	gint type;
	union _Value def;
	gpointer value;
};

typedef union _Value Value;

struct {
	gboolean away_auto_unaway;
	gint     away_check_limit;
	gboolean away_color_away;
	gint     away_color_limit;
	gchar   *away_reason;
	gboolean away_show_away;
	gboolean away_show_once;
	gboolean channel_show_key;
	gboolean channel_topicbar;
	gboolean channel_usercount;
	gint     completion_amount;
	gboolean completion_auto;
	gboolean completion_sort;
	gchar   *completion_suffix;
	gint     dcc_autoaccept_mode;
	gboolean dcc_autoresume;
	gint     dcc_blocksize;
	gboolean dcc_chat_autoaccept;
	gchar   *dcc_downloaded_dir;
	gboolean dcc_escape_special;
	gint     dcc_file_max_in;
	gint     dcc_file_max_out;
	gint     dcc_global_max_in;
	gint     dcc_global_max_out;
	gchar   *dcc_hostname;
	gboolean dcc_nick_in_filename;
	gchar   *dcc_permissions;
	gchar   *dcc_ports;
	gboolean dcc_remove_completed;
	gboolean dcc_server_ip;
	gint     dcc_stall_timeout;
	gchar   *dcc_tempdir;
	gint     dcc_timeout;
	gint     flood_ctcp_count;
	gint     flood_ctcp_seconds;
	gboolean flood_enable;
	gint     flood_privmsg_count;
	gint     flood_privmsg_seconds;
	gboolean gui_autoopen_dcc_chat;
	gboolean gui_autoopen_dcc_send;
	gboolean gui_autoopen_dcc_get;
	gboolean gui_autoopen_query;
	gchar   *gui_background_image;
	gboolean gui_beep_channel;
	gboolean gui_beep_hilight;
	gboolean gui_beep_private;
	gboolean gui_channel_tabs;
	gint     gui_dialog_height;
	gint     gui_dialog_left;
	gint     gui_dialog_top;
	gint     gui_dialog_width;
	gboolean gui_flash_channel;
	gboolean gui_flash_hilight;
	gboolean gui_flash_private;
	gchar   *gui_font;
	gboolean gui_hide_menu;
	gint     gui_lagometer;
	gint     gui_left_pane_size;
	gboolean gui_maximized;
	gboolean gui_prompt_quit;
	gboolean gui_query_tabs;
	gint     gui_right_pane_size;
	gint     gui_selected_network;
	gboolean gui_server_tabs;
	gboolean gui_show_marker;
	gboolean gui_skip_network_list;
	gboolean gui_snotice_tabs;
	gboolean gui_state_save;
	gboolean gui_style_inputbox;
	gboolean gui_swap_panes;
	gboolean gui_thin_separator;
	gint     gui_throttlemeter;
	gchar   *gui_tint_rgb;
	gboolean gui_transparent;
	gboolean gui_treeview;
	gboolean gui_utility_tabs;
	gint     gui_window_height;
	gint     gui_window_left;
	gint     gui_window_top;
	gint     gui_window_width;
	gint     irc_bantype;
	gchar   *irc_default_part;
	gchar   *irc_default_quit;
	gboolean irc_hide_jpq;
	gboolean irc_hide_version;
	gchar   *irc_nick1;
	gchar   *irc_nick2;
	gchar   *irc_nick3;
	gchar   *irc_real_name;
	gboolean irc_rejoin_on_kick;
	gboolean irc_skip_motd;
	gchar   *irc_user_name;
	gchar   *irc_user_modes;
	gboolean irc_who_join;
	gboolean irc_whois_active;
	gboolean log_auto;
	gchar   *log_auto_format;
	gboolean log_timestamp;
	gchar   *log_timestamp_format;
	gboolean misc_autosave;
	gboolean misc_autosave_url;
	gchar   *misc_sound_command;
	gint     misc_url_modkey;
	gint     notify_timeout;
	gboolean notify_whois_online;
	gchar   *proxy_hostname;
	gchar   *proxy_password;
	gint     proxy_port;
	gint     proxy_type;
	gboolean proxy_use_dcc;
	gboolean proxy_use_irc;
	gchar   *proxy_user;
	gboolean server_autoreconnect;
	gchar   *server_hostname;
	gint     server_ping_timeout;
	gint     server_reconnect_delay;
	gboolean server_reconnect_on_fail;
	gboolean text_color_nicks;
	gchar   *text_commandchar;
	gboolean text_filter_beep;
	gboolean text_interpret_ascii;
	gboolean text_interpret_format;
	gboolean text_reload_buffer;
	gint     text_scrollback;
	gint     text_space_indent;
	gboolean text_spellcheck;
	gboolean text_strip_color;
	gboolean text_timestamp;
	gchar   *text_timestamp_format;
	gboolean text_utf8_locale;
	gboolean text_wordwrap;
	gboolean tray_blink_channel;
	gboolean tray_blink_hilight;
	gboolean tray_blink_private;
	gboolean tray_enable;
	gboolean tray_minimize_on_exit;
	gboolean tray_minimize_to_tray;
	gboolean tray_notify_channel;
	gboolean tray_notify_hilight;
	gboolean tray_notify_only_when_hidden;
	gboolean tray_notify_private;
	gint     tab_focus_new;
	gint     tab_name_length;
	gint     tab_position;
	gint     tab_size;
	gboolean tab_sort;
	gboolean treeview_icons;
	gboolean treeview_no_connecting_lines;
	gboolean userlist_buttons;
	gchar   *userlist_dclick_action;
	gboolean userlist_hide;
	gint     userlist_position;
	gboolean userlist_resizable;
	gboolean userlist_show_hosts;
	gint     userlist_sort_order;
	gboolean userlist_style;
} prefs;

void config_load(ConfigDb *config);
void config_save(ConfigDb *config);
void config_default(const gchar *section, const gchar *key, Value *val);

#endif /* DEFCONF_H */
