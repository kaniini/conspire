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

#include "defconf.h"
#include "configdb.h"
#include <glib.h>
#include <string.h>
#include <stdio.h>

struct ConfPrefs prefs;

struct Config conf_vt[] = {
	/* Away options */
	{"away",   "auto_unaway", CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.away_auto_unaway}},
	{"away",   "check_limit", CONF_INT,  {intv:  60},    {intv: &prefs.away_check_limit}},
	{"away",   "color_away",  CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.away_color_away}},
	{"away",   "color_limit", CONF_INT,  {intv:  300},   {intv: &prefs.away_color_limit}},
	{"away",   "reason",      CONF_STR,  {strv:  "I am currently away from my computer."}, {strv: &prefs.away_reason}},
	{"away",   "show_away",   CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.away_show_away}},
	{"away",   "show_once",   CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.away_show_once}},

	/* Channel options */
	{"channel", "show_key",   CONF_BOOL, {boolv: TRUE}, {boolv: &prefs.channel_show_key}},
	{"channel", "topicbar",   CONF_BOOL, {boolv: TRUE}, {boolv: &prefs.channel_topicbar}},
	{"channel", "usercount",  CONF_BOOL, {boolv: TRUE}, {boolv: &prefs.channel_usercount}},

	/* Tab-completion options */
	{"completion", "amount", CONF_INT,  {intv:  5},     {intv: &prefs.completion_amount}},
	{"completion", "auto",   CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.completion_auto}},
	{"completion", "sort",   CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.completion_sort}},
	{"completion", "suffix", CONF_STR,  {strv:  ","},   {strv: &prefs.completion_suffix}},

	/* DCC options */
	{"dcc", "autoaccept_mode",  CONF_INT,  {intv:  0},     {intv: &prefs.dcc_autoaccept_mode}},
	{"dcc", "autoresume",       CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.dcc_autoresume}},
	{"dcc", "blocksize",        CONF_INT,  {intv:  1024},  {intv: &prefs.dcc_blocksize}},
	{"dcc", "chat_autoaccept",  CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.dcc_chat_autoaccept}},
	{"dcc", "downloaded_dir",   CONF_STR,  {strv:  ""},    {strv: &prefs.dcc_downloaded_dir}},
	{"dcc", "escape_special",   CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.dcc_escape_special}},
	{"dcc", "file_max_in",      CONF_INT,  {intv:  0},     {intv: &prefs.dcc_file_max_in}},
	{"dcc", "file_max_out",     CONF_INT,  {intv:  0},     {intv: &prefs.dcc_file_max_out}},
	{"dcc", "global_max_in",    CONF_INT,  {intv:  0},     {intv: &prefs.dcc_global_max_in}},
	{"dcc", "global_max_out",   CONF_INT,  {intv:  0},     {intv: &prefs.dcc_global_max_out}},
	{"dcc", "hostname",         CONF_STR,  {strv:  ""},    {strv: &prefs.dcc_hostname}},
	{"dcc", "nick_in_filename", CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.dcc_nick_in_filename}},
	{"dcc", "permissions",      CONF_STR,  {strv:  "644"}, {strv: &prefs.dcc_permissions}},       
	{"dcc", "ports",            CONF_STR,  {strv:  ""},    {strv: &prefs.dcc_ports}},             
	{"dcc", "remove_completed", CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.dcc_remove_completed}},  
	{"dcc", "server_ip",        CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.dcc_server_ip}},         
	{"dcc", "stall_timeout",    CONF_INT,  {intv:  180},   {intv: &prefs.dcc_stall_timeout}},     
	{"dcc", "tempdir",          CONF_STR,  {strv:  "/tmp/conspire"}, {strv: &prefs.dcc_tempdir}}, 
	{"dcc", "timeout",          CONF_INT,  {intv:  180},   {intv: &prefs.dcc_timeout}},           
	
	/* Flood-control options */
	{"flood", "ctcp_count",      CONF_INT,  {intv:  10},   {intv: &prefs.flood_ctcp_count}},       
	{"flood", "ctcp_seconds",    CONF_INT,  {intv:  3},    {intv: &prefs.flood_ctcp_seconds}},     
	{"flood", "enable",          CONF_BOOL, {boolv: TRUE}, {boolv: &prefs.flood_enable}},           
	{"flood", "privmsg_count",   CONF_INT,  {intv:  10},   {intv: &prefs.flood_privmsg_count}},    
	{"flood", "privmsg_seconds", CONF_INT,  {intv:  3},    {intv: &prefs.flood_privmsg_seconds}},  

	/* GUI options */
	{"gui", "autoopen_dcc_chat", CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_autoopen_dcc_chat}},  
	{"gui", "autoopen_dcc_get",  CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_autoopen_dcc_get}},   
	{"gui", "autoopen_dcc_send", CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_autoopen_dcc_send}},  
	{"gui", "autoopen_query",    CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_autoopen_query}},     
	{"gui", "background_image",  CONF_STR,  {strv:  ""},    {strv: &prefs.gui_background_image}},   
	{"gui", "beep_channel",      CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_beep_channel}},       
	{"gui", "beep_hilight",      CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_beep_hilight}},       
	{"gui", "beep_private",      CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_beep_private}},       
	{"gui", "channel_tabs",      CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_channel_tabs}},       
	{"gui", "dialog_height",     CONF_INT,  {intv:  256},   {intv: &prefs.gui_dialog_height}},      
	{"gui", "dialog_left",       CONF_INT,  {intv:  0},     {intv: &prefs.gui_dialog_left}},        
	{"gui", "dialog_top",        CONF_INT,  {intv:  0},     {intv: &prefs.gui_dialog_top}},         
	{"gui", "dialog_width",      CONF_INT,  {intv:  512},   {intv: &prefs.gui_dialog_width}},       
	{"gui", "flash_channel",     CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_flash_channel}},      
	{"gui", "flash_hilight",     CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_flash_hilight}},      
	{"gui", "flash_private",     CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_flash_private}},      
	{"gui", "font",              CONF_STR,  {strv:  "Monospace 9"}, {strv: &prefs.gui_font}},       
	{"gui", "hide_menu",         CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_hide_menu}},          
	{"gui", "lagometer",         CONF_INT,  {intv:  2},     {intv: &prefs.gui_lagometer}},          
	{"gui", "left_pane_size",    CONF_INT,  {intv:  100},   {intv: &prefs.gui_left_pane_size}},     
	{"gui", "maximized",         CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_maximized}},          
	{"gui", "prompt_quit",       CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_prompt_quit}},        
	{"gui", "query_tabs",        CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_query_tabs}},         
	{"gui", "right_pane_size",   CONF_INT,  {intv:  100},   {intv: &prefs.gui_right_pane_size}},    
	{"gui", "selected_network",  CONF_INT,  {intv:  0},     {intv: &prefs.gui_selected_network}},   
	{"gui", "server_tabs",       CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_server_tabs}},        
	{"gui", "show_marker",       CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_show_marker}},        
	{"gui", "skip_network_list", CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_skip_network_list}},  
	{"gui", "snotice_tabs",      CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_snotice_tabs}},       
	{"gui", "state_save",        CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_state_save}},         
	{"gui", "style_inputbox",    CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_style_inputbox}},     
	{"gui", "swap_panes",        CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_swap_panes}},         
	{"gui", "thin_separator",    CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_thin_separator}},     
	{"gui", "throttlemeter",     CONF_INT,  {intv:  2},     {intv: &prefs.gui_throttlemeter}},      
	{"gui", "tint_rgb",          CONF_STR,  {strv:  "#C3C3C3"}, {strv: &prefs.gui_tint_rgb}},       
	{"gui", "transparent",       CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_transparent}},        
	{"gui", "treeview",          CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.gui_treeview}},           
	{"gui", "utility_tabs",      CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.gui_utility_tabs}},       
	{"gui", "window_height",     CONF_INT,  {intv:  400},   {intv: &prefs.gui_window_height}},      
	{"gui", "window_left",       CONF_INT,  {intv:  0},     {intv: &prefs.gui_window_left}},        
	{"gui", "window_top",        CONF_INT,  {intv:  0},     {intv: &prefs.gui_window_top}},         
	{"gui", "window_width",      CONF_INT,  {intv:  640},   {intv: &prefs.gui_window_width}},       

	/* IRC-specific options */
	{"irc", "bantype",        CONF_INT,  {intv:  0},               {intv: &prefs.irc_bantype}},
	{"irc", "default_part",   CONF_STR,  {strv: "Leaving."},       {strv: &prefs.irc_default_part}},
	{"irc", "default_quit",   CONF_STR,  {strv: "Leaving."},       {strv: &prefs.irc_default_quit}},
	{"irc", "hide_jpq",       CONF_BOOL, {boolv: FALSE},           {boolv: &prefs.irc_hide_jpq}},       
	{"irc", "hide_version",   CONF_BOOL, {boolv: FALSE},           {boolv: &prefs.irc_hide_version}},   
	{"irc", "nick1",          CONF_STR,  {strv:  ""},              {strv: &prefs.irc_nick1}},          
	{"irc", "nick2",          CONF_STR,  {strv:  ""},              {strv: &prefs.irc_nick2}},          
	{"irc", "nick3",          CONF_STR,  {strv:  ""},              {strv: &prefs.irc_nick3}},          
	{"irc", "real_name",      CONF_STR,  {strv:  ""},              {strv: &prefs.irc_real_name}},      
	{"irc", "rejoin_on_kick", CONF_BOOL, {boolv: FALSE},           {boolv: &prefs.irc_rejoin_on_kick}}, 
	{"irc", "skip_motd",      CONF_BOOL, {boolv: FALSE},           {boolv: &prefs.irc_skip_motd}},      
	{"irc", "user_name",      CONF_STR,  {strv:  ""},              {strv: &prefs.irc_user_name}},      
	{"irc", "user_modes",     CONF_STR,  {strv:  ""},              {strv: &prefs.irc_user_modes}},     
	{"irc", "who_join",       CONF_BOOL, {boolv: FALSE},           {boolv: &prefs.irc_who_join}},       
	{"irc", "whois_active",   CONF_BOOL, {boolv: TRUE},            {boolv: &prefs.irc_whois_active}},   

	/* Loggiong options */
	{"log", "auto",             CONF_BOOL, {boolv: FALSE},      {boolv: &prefs.log_auto}},
	{"log", "auto_format",      CONF_STR,  {strv:  "logs/$network/$channel.log"}, {strv: &prefs.log_auto_format}},
	{"log", "timestamp",        CONF_BOOL, {boolv: TRUE},       {boolv: &prefs.log_timestamp}},
	{"log", "timestamp_format", CONF_STR,  {strv:  "%H:%M%:S"}, {strv: &prefs.log_timestamp_format}},

	/* Miscellaenous options */
	{"misc", "autosave",      CONF_BOOL, {boolv: TRUE},  {boolv: &prefs.misc_autosave}},
	{"misc", "autosave_url",  CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.misc_autosave_url}},
	{"misc", "sound_command", CONF_STR,  {strv:  ""},    {strv: &prefs.misc_sound_command}},
	{"misc", "url_modkey",    CONF_INT,  {intv:  4},     {intv: &prefs.misc_url_modkey}},

	/* Notify options */
	{"notify", "timeout",      CONF_INT,  {intv:  30},    {intv: &prefs.notify_timeout}},       /* Check if users are online every n seconds */
	{"notify", "whois_online", CONF_BOOL, {boolv: FALSE}, {boolv: &prefs.notify_whois_online}},  /* WHOIS users on notify */

	/* Proxy options */
	{"proxy", "hostname",   CONF_STR,  {strv:  ""},    &prefs.proxy_hostname},  /* Hostname to proxy connections through */
	{"proxy", "password",   CONF_STR,  {strv:  ""},    &prefs.proxy_password},  /* Password to authenticate to the proxy with */
	{"proxy", "port",       CONF_INT,  {intv:  0},     &prefs.proxy_port},      /* Port to use when using the proxy */
	{"proxy", "type",       CONF_INT,  {intv:  0},     &prefs.proxy_type},      /* Type of proxy; 0 = disabled, 1 = wingate, 2 = SOCKS4, 3 = SOCKS5, 4 = HTTP, 5 = ISA */
	{"proxy", "use_dcc",    CONF_BOOL, {boolv: FALSE}, &prefs.proxy_use_dcc},   /* Use the proxy for DCC connections */
	{"proxy", "use_irc",    CONF_BOOL, {boolv: FALSE}, &prefs.proxy_use_irc},   /* Use the proxy for IRC connections */
	{"proxy", "user",       CONF_STR,  {strv:  ""},    &prefs.proxy_user},      /* Username to authenticate to the proxy with */

	/* Server options */
	{"server", "autoreconnect",     CONF_BOOL, {boolv: TRUE},  &prefs.server_autoreconnect},      /* Autoreconnect to server when disconnected */
	{"server", "hostname",          CONF_STR,  {strv:  ""},    &prefs.server_hostname},           /* Address/hostname to bind to when connecting to servers */
	{"server", "ping_timeout",      CONF_INT,  {intv:  300},   &prefs.server_ping_timeout},       /* How long to wait before assuming a dropped connection */
	{"server", "reconnect_delay",   CONF_INT,  {intv:  30},    &prefs.server_reconnect_delay},    /* Length of time to wait between reconnect attempts. */
	{"server", "reconnect_on_fail", CONF_BOOL, {boolv: FALSE}, &prefs.server_reconnect_on_fail},  /* Attempt to connect after a failed connection attempt */

	/* Text options */
	{"text", "color_nicks",      CONF_BOOL, {boolv: FALSE},   &prefs.text_color_nicks},       /* Colour nicks */
	{"text", "commandchar",      CONF_STR,  {strv:  "/"},     &prefs.text_commandchar},       /* Character to strip when processing commands */
	{"text", "filter_beep",      CONF_BOOL, {boolv: TRUE},    &prefs.text_filter_beep},       /* Filter ANSI bell character [^G] */
	{"text", "interpret_ascii",  CONF_BOOL, {boolv: TRUE},    &prefs.text_interpret_ascii},   /* Interpret %nnn into an ASCII character */
	{"text", "interpret_format", CONF_BOOL, {boolv: TRUE},    &prefs.text_interpret_format},  /* Interpret %C, %B, etc. as appropriate controlcodes */
	{"text", "reload_buffer",    CONF_BOOL, {boolv: TRUE},    &prefs.text_reload_buffer},     /* Load previous contents of channels when rejoining */
	{"text", "scrollback",       CONF_INT,  {intv:  1000},    &prefs.text_scrollback},        /* Number of lines in scrollback buffer */
	{"text", "space_indent",     CONF_INT,  {intv:  10},      &prefs.text_space_indent},      /* How many spaces to indent wrapping text with */
	{"text", "spellcheck",       CONF_BOOL, {boolv: TRUE},    &prefs.text_spellcheck},        /* Spellcheck your text, lulz */
	{"text", "strip_color",      CONF_BOOL, {boolv: FALSE},   &prefs.text_strip_color},       /* Strip IRC colours */
	{"text", "timestamp",        CONF_BOOL, {boolv: TRUE},    &prefs.text_timestamp},         /* Timestamp text */
	{"text", "timestamp_format", CONF_STR,  {strv:  "%H:%M"}, &prefs.text_timestamp_format},  /* Timestamp format */
	{"text", "utf8_locale",      CONF_BOOL, {boolv: TRUE},    &prefs.text_utf8_locale},       /* UTF-8 locale defined */
	{"text", "wordwrap",         CONF_BOOL, {boolv: TRUE},    &prefs.text_wordwrap},          /* Wrap overly long messages */
	
	/* System tray icon options */
	{"tray", "blink_channel",           CONF_BOOL, {boolv: FALSE}, &prefs.tray_blink_channel},            /* Blink tray icon when a channel message is received */
	{"tray", "blink_hilight",           CONF_BOOL, {boolv: TRUE},  &prefs.tray_blink_hilight},            /* Blink tray icon when a hilight message is received */
	{"tray", "blink_private",           CONF_BOOL, {boolv: TRUE},  &prefs.tray_blink_private},            /* Blink tray icon when a private message is received */
	{"tray", "enable",                  CONF_BOOL, {boolv: TRUE},  &prefs.tray_enable},                   /* Enable system tray icon */
	{"tray", "minimize_on_exit",        CONF_BOOL, {boolv: FALSE}, &prefs.tray_minimize_on_exit},         /* Hide to tray icon on exit */
	{"tray", "minimize_to_tray",        CONF_BOOL, {boolv: FALSE}, &prefs.tray_minimize_to_tray},         /* Minimize to tray instead of taskbar */
	{"tray", "notify_channel",          CONF_BOOL, {boolv: FALSE}, &prefs.tray_notify_channel},           /* Notify when a channel message is received */
	{"tray", "notify_hilight",          CONF_BOOL, {boolv: FALSE}, &prefs.tray_notify_hilight},           /* Notify when a hilight message is received */
	{"tray", "notify_only_when_hidden", CONF_BOOL, {boolv: FALSE}, &prefs.tray_notify_only_when_hidden},  /* Only show notifications when minimized to tray */
	{"tray", "notify_private",          CONF_BOOL, {boolv: FALSE}, &prefs.tray_notify_private},           /* Notify when a private message is received */

	/* Tab options */
	{"tab", "focus_new",   CONF_INT,  {intv:  1},     &prefs.tab_focus_new},    /* When to focus new tabs; 0 = never, 1 = always, 2 = only on request */
	{"tab", "name_length", CONF_INT,  {intv:  11},    &prefs.tab_name_length},  /* Truncate channel names after n many characters */
	{"tab", "position",    CONF_INT,  {intv:  4},     &prefs.tab_position},     /* Where to place tabs; 1 = upper left, 2 = left, 3 = upper right, 4 = right, 5 = top, 6 = bottom, 7 = hidden */
	{"tab", "size",        CONF_INT,  {intv:  0},     &prefs.tab_size},         /* Size of tabs; 0 = normal, 1 = small, 2 = extra small */
	{"tab", "sort",        CONF_BOOL, {boolv: FALSE}, &prefs.tab_sort},         /* Sort tabs alphabetically */
	
	/* Treeview options */
	{"treeview", "icons",               CONF_BOOL, {boolv: FALSE}, &prefs.treeview_icons},                /* Use icons in treeview mode */
	{"treeview", "no_connecting_lines", CONF_BOOL, {boolv: FALSE}, &prefs.treeview_no_connecting_lines},  /* Disable dotted connecting lines */

	/* Userlist options */
	{"userlist", "buttons",       CONF_BOOL, {boolv: TRUE},          &prefs.userlist_buttons},        /* Show userlist buttons */
	{"userlist", "dclick_action", CONF_STR,  {strv:  "whois %s %s"}, &prefs.userlist_dclick_action},  /* Command to run when doubleclicking on usernames */
	{"userlist", "hide",          CONF_BOOL, {boolv: FALSE},         &prefs.userlist_hide},           /* Hide userlist */
	{"userlist", "position",      CONF_INT,  {intv:  3},             &prefs.userlist_position},       /* Userlist position; 1 = upper left, 2 = lower left, 3 = upper right, 4 = lower right */
	{"userlist", "resizable",     CONF_BOOL, {boolv: TRUE},          &prefs.userlist_resizable},      /* Allow userlist to be resizable */
	{"userlist", "show_hosts",    CONF_BOOL, {boolv: FALSE},         &prefs.userlist_show_hosts},     /* Show hostnames in userlist */
	{"userlist", "sort_order",    CONF_INT,  {intv:  0},             &prefs.userlist_sort_order},     /* Sort userlist. 0 = A-Z, moded first; 1 = A-Z; 2 = A-Z, moded last; 3 = Z-A; 4 = unsorted */
	{"userlist", "style",         CONF_BOOL, {boolv: TRUE},          &prefs.userlist_style},          /* Style the userlist a la the main view */
	{NULL}
};

void config_load(ConfigDb *config) {
	gint iter = 0;
	gchar *strv;
	gboolean boolv;
	gint intv;
	gfloat floatv;
	gdouble doublev;

	while (conf_vt[iter].section != NULL) {
		switch (conf_vt[iter].type) {
			case CONF_STR:
				{
					if (!settings_get_string(config, conf_vt[iter].section, conf_vt[iter].key, &strv))
						strv = g_strdup(conf_vt[iter].def.strv);

					*conf_vt[iter].value.strv = strv;

					g_print("[conf] %s::%s = %s\n", conf_vt[iter].section, conf_vt[iter].key,
						*conf_vt[iter].value.strv);
				}
				break;
			case CONF_BOOL:
				{
					if (!settings_get_bool(config, conf_vt[iter].section, conf_vt[iter].key, &boolv))
						boolv = conf_vt[iter].def.boolv;

					*conf_vt[iter].value.boolv = boolv;
				}
				break;
			case CONF_INT:
				{
					if (!settings_get_int(config, conf_vt[iter].section, conf_vt[iter].key, &intv))
						intv = conf_vt[iter].def.intv;

					*conf_vt[iter].value.intv = intv;
				}
				break;
			case CONF_FLOAT:
				{
					if (!settings_get_float(config, conf_vt[iter].section, conf_vt[iter].key, &floatv))
						floatv = conf_vt[iter].def.floatv;

					*conf_vt[iter].value.floatv = floatv;
				}
				break;
			case CONF_DOUBLE:
				{
					if (!settings_get_double(config, conf_vt[iter].section, conf_vt[iter].key, &doublev))
						doublev = conf_vt[iter].def.doublev;

					*conf_vt[iter].value.doublev = doublev;
				}
				break;
			default:
				printf("config_load: unknown type for %s::%s: %d", conf_vt[iter].section, conf_vt[iter].key, conf_vt[iter].type);
				break;
		}
		iter++;
	}
}

void config_save(ConfigDb *config) {
	gint iter = 0;
	gchar *strv;
	gboolean boolv;
	gint intv;
	gfloat floatv;
	gdouble doublev;

	while (conf_vt[iter].section != NULL) {
		switch (conf_vt[iter].type) {
			case CONF_STR:
				strv = g_strdup(*conf_vt[iter].value.strv);
				settings_set_string(config, conf_vt[iter].section, conf_vt[iter].key, strv);
				break;
			case CONF_BOOL:
				boolv = *conf_vt[iter].value.boolv;
				settings_set_bool(config, conf_vt[iter].section, conf_vt[iter].key, boolv);
				break;
			case CONF_INT:
				intv = *conf_vt[iter].value.intv;
				settings_set_int(config, conf_vt[iter].section, conf_vt[iter].key, intv);
				break;
			case CONF_FLOAT:
				floatv = *conf_vt[iter].value.floatv;
				settings_set_float(config, conf_vt[iter].section, conf_vt[iter].key, floatv);
				break;
			case CONF_DOUBLE:
				doublev = *conf_vt[iter].value.doublev;
				settings_set_double(config, conf_vt[iter].section, conf_vt[iter].key, doublev);
				break;
			default:
				printf("config_save: unknown type for %s::%s: %d", conf_vt[iter].section, conf_vt[iter].key, conf_vt[iter].type);
				break;
		}
		iter++;
	}
}

/* return default value for a given section::key */
void config_def_string(const gchar *section, const gchar *key, Value *val) {
	gint iter = 0;

	while (&conf_vt[iter]) {
		if (strcmp(section, conf_vt[iter].section) && strcmp(key, conf_vt[iter].key)) {
			memcpy(val, &conf_vt[iter].def, sizeof(conf_vt[iter].def));
			break;
		}
		iter++;
	}
}
