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

struct Config conf_vt[] = {
	/* Away options */
	{"away",   "auto_unaway", CONF_BOOL, {boolv: FALSE}, &prefs.away_auto_unaway},  /* Automatically unaway when sending a message while away */
	{"away",   "check_limit", CONF_INT,  {intv:  60},    &prefs.away_check_limit},  /* How often to check for away users for colouring purposes */
	{"away",   "color_away",  CONF_BOOL, {boolv: TRUE},  &prefs.away_color_away},   /* Colour away users' nicks */
	{"away",   "color_limit", CONF_INT,  {intv:  300},   &prefs.away_color_limit},  /* Maximum number of away users before ceasing to colour them */
	{"away",   "reason",      CONF_STR,  {strv:  "I am currently away from my computer."}, &prefs.away_reason},  /* Default away message */
	{"away",   "show_away",   CONF_BOOL, {boolv: FALSE}, &prefs.away_show_away},    /* Show away message */
	{"away",   "show_once",   CONF_BOOL, {boolv: FALSE}, &prefs.away_show_once},    /* Show identical away messages only once */

	/* Channel options */
	{"channel", "show_key",   CONF_BOOL, {boolv: TRUE}, &prefs.channel_show_key},   /* Show channel modes in titlebar when key is set */
	{"channel", "topicbar",   CONF_BOOL, {boolv: TRUE}, &prefs.channel_topicbar},   /* Show topicbar */
	{"channel", "usercount",  CONF_BOOL, {boolv: TRUE}, &prefs.channel_usercount},  /* Show usercount in titlebar */

	/* Tab-completion options */
	{"completion", "amount", CONF_INT,  {intv:  5},     &prefs.completion_amount},  /* How many nicks must match before showing all */
	{"completion", "auto",   CONF_BOOL, {boolv: FALSE}, &prefs.completion_auto},    /* Automatically complete nicks before sending to channel */
	{"completion", "sort",   CONF_BOOL, {boolv: TRUE},  &prefs.completion_sort},    /* Whether to sort by last-talked or not */
	{"completion", "suffix", CONF_STR,  {strv:  ","},   &prefs.completion_suffix},  /* Suffix to use for autocompletion checks */

	/* DCC options */
	{"dcc", "autoaccept_mode",  CONF_INT,  {intv:  0},     &prefs.dcc_autoaccept_mode},   /* Auto-accept DCC transfers: 0 = don't accept, 1 = auto-accept, 2 = browse */
	{"dcc", "autoresume",       CONF_BOOL, {boolv: FALSE}, &prefs.dcc_autoresume},        /* Automatically resume DCC transfers instead of overwriting */
	{"dcc", "blocksize",        CONF_INT,  {intv:  1024},  &prefs.dcc_blocksize},         /* Blocksize for DCC transfers */
	{"dcc", "chat_autoaccept",  CONF_BOOL, {boolv: FALSE}, &prefs.dcc_chat_autoaccept},   /* Auto-accept DCC Chats */
	{"dcc", "downloaded_dir",   CONF_STR,  {strv:  ""},    &prefs.dcc_downloaded_dir},    /* Directory to place completed transfers in */
	{"dcc", "escape_special",   CONF_BOOL, {boolv: FALSE}, &prefs.dcc_escape_special},    /* Replace spaces and non-7-bit-clean characters with underscores */
	{"dcc", "file_max_in",      CONF_INT,  {intv:  0},     &prefs.dcc_file_max_in},       /* Per-file maximum inbound transfer limit in bytes; 0 = unlimited */
	{"dcc", "file_max_out",     CONF_INT,  {intv:  0},     &prefs.dcc_file_max_out},      /* Per-file maximum outbound transfer limit in bytes; 0 = unlimited */
	{"dcc", "global_max_in",    CONF_INT,  {intv:  0},     &prefs.dcc_global_max_in},     /* Global maximum inbound transfer limit in bytes; 0 = unlimited */
	{"dcc", "global_max_out",   CONF_INT,  {intv:  0},     &prefs.dcc_global_max_out},    /* Global maximum outbound transfer limit in bytes; 0 = unlimited */
	{"dcc", "hostname",         CONF_STR,  {strv:  ""},    &prefs.dcc_hostname},          /* Address/hostname to bind to for DCC transfers */
	{"dcc", "nick_in_filename", CONF_BOOL, {boolv: FALSE}, &prefs.dcc_nick_in_filename},  /* Use nick in received transfers' filenames */
	{"dcc", "permissions",      CONF_STR,  {strv:  "644"}, &prefs.dcc_permissions},       /* UNIX permissions in octal; see chmod(1) for more information */
	{"dcc", "ports",            CONF_STR,  {strv:  ""},    &prefs.dcc_ports},             /* List of DCC port[ range]s */
	{"dcc", "remove_completed", CONF_BOOL, {boolv: TRUE},  &prefs.dcc_remove_completed},  /* Automatically remove completed transfers from the DCC list */
	{"dcc", "server_ip",        CONF_BOOL, {boolv: FALSE}, &prefs.dcc_server_ip},         /* Get address from IRC server */
	{"dcc", "stall_timeout",    CONF_INT,  {intv:  180},   &prefs.dcc_stall_timeout},     /* Length of time to wait before timing out during a transfer */
	{"dcc", "tempdir",          CONF_STR,  {strv:  "/tmp/conspire"}, &prefs.dcc_tempdir}, /* Directory to place in-progress transfers in */
	{"dcc", "timeout",          CONF_INT,  {intv:  180},   &prefs.dcc_timeout},           /* Length of time to wait before timing out tranfers waiting to be accepted */
	
	/* Flood-control options */
	{"flood", "ctcp_count",      CONF_INT,  {intv:  10},   &prefs.flood_ctcp_count},       /* Number of CTCPs in a given timespan before considering it a flood */
	{"flood", "ctcp_seconds",    CONF_INT,  {intv:  3},    &prefs.flood_ctcp_seconds},     /* Length of time to count CTCPs for flood control */
	{"flood", "enable",          CONF_BOOL, {boolv: TRUE}, &prefs.flood_enable},           /* Enable flood control */
	{"flood", "privmsg_count",   CONF_INT,  {intv:  10},   &prefs.flood_privmsg_count},    /* Number of messages in a given timespan before considering it a flood */
	{"flood", "privmsg_seconds", CONF_INT,  {intv:  3},    &prefs.flood_privmsg_seconds},  /* Length of time to count messages for flood control */

	/* GUI options */
	{"gui", "autoopen_dcc_chat", CONF_BOOL, {boolv: FALSE}, &prefs.gui_autoopen_dcc_chat},  /* Automatically open DCC chats */
	{"gui", "autoopen_dcc_get",  CONF_BOOL, {boolv: FALSE}, &prefs.gui_autoopen_dcc_get},   /* Automatically open DCC dialog on receiving a file */
	{"gui", "autoopen_dcc_send", CONF_BOOL, {boolv: FALSE}, &prefs.gui_autoopen_dcc_send},  /* Automatically open DCC dialog on sending a file */
	{"gui", "autoopen_query",    CONF_BOOL, {boolv: TRUE},  &prefs.gui_autoopen_query},     /* Automatically open queries */
	{"gui", "background_image",  CONF_STR,  {strv:  ""},    &prefs.gui_background_image},   /* Background image for the main view */
	{"gui", "beep_channel",      CONF_BOOL, {boolv: FALSE}, &prefs.gui_beep_channel},       /* Beep when a channel message is received */
	{"gui", "beep_hilight",      CONF_BOOL, {boolv: TRUE},  &prefs.gui_beep_hilight},       /* Beep when a hilight message is received */
	{"gui", "beep_private",      CONF_BOOL, {boolv: TRUE},  &prefs.gui_beep_private},       /* Beep when a private message is received */
	{"gui", "channel_tabs",      CONF_BOOL, {boolv: TRUE},  &prefs.gui_channel_tabs},       /* Use tabs instead of windows for channels */
	{"gui", "dialog_height",     CONF_INT,  {intv:  256},   &prefs.gui_dialog_height},      /* Default dialog height */
	{"gui", "dialog_left",       CONF_INT,  {intv:  0},     &prefs.gui_dialog_left},        /* Default dialog Y position */
	{"gui", "dialog_top",        CONF_INT,  {intv:  0},     &prefs.gui_dialog_top},         /* Default dialog X position */
	{"gui", "dialog_width",      CONF_INT,  {intv:  512},   &prefs.gui_dialog_width},       /* Default dialog width */
	{"gui", "flash_channel",     CONF_BOOL, {boolv: FALSE}, &prefs.gui_flash_channel},      /* Flash taskbar entry when a channel message is received */
	{"gui", "flash_hilight",     CONF_BOOL, {boolv: TRUE},  &prefs.gui_flash_hilight},      /* Flash taskbar entry when a hilight message is received */
	{"gui", "flash_private",     CONF_BOOL, {boolv: TRUE},  &prefs.gui_flash_private},      /* Flash taskbar entry when a private message is received */
	{"gui", "font",              CONF_STR,  {strv:  "Monospace 9"}, &prefs.gui_font},       /* Font to use for text in the main view */
	{"gui", "hide_menu",         CONF_BOOL, {boolv: FALSE}, &prefs.gui_hide_menu},          /* Hide menubar */
	{"gui", "lagometer",         CONF_INT,  {intv:  2},     &prefs.gui_lagometer},          /* Lagometer's appareance. 0 = disable, 1 = bar, 2 = text, 3 = both */
	{"gui", "left_pane_size",    CONF_INT,  {intv:  100},   &prefs.gui_left_pane_size},     /* Size of left pane [usually server/channel list] */
	{"gui", "maximized",         CONF_BOOL, {boolv: FALSE}, &prefs.gui_maximized},          /* Maximize window on startup */
	{"gui", "prompt_quit",       CONF_BOOL, {boolv: TRUE},  &prefs.gui_prompt_quit},        /* Prompt to quit if connected to servers */
	{"gui", "query_tabs",        CONF_BOOL, {boolv: TRUE},  &prefs.gui_query_tabs},         /* Use tabs instead of windows for queries */
	{"gui", "right_pane_size",   CONF_INT,  {intv:  100},   &prefs.gui_right_pane_size},    /* Size of right pane [usually userlist] */
	{"gui", "selected_network",  CONF_INT,  {intv:  0},     &prefs.gui_selected_network},   /* Selected network in network list */
	{"gui", "server_tabs",       CONF_BOOL, {boolv: FALSE}, &prefs.gui_server_tabs},        /* Create new tabs for server messages */
	{"gui", "show_marker",       CONF_BOOL, {boolv: TRUE},  &prefs.gui_show_marker},        /* Show last line the user is likely to have read */
	{"gui", "skip_network_list", CONF_BOOL, {boolv: FALSE}, &prefs.gui_skip_network_list},  /* Skip network list on startup */
	{"gui", "snotice_tabs",      CONF_BOOL, {boolv: FALSE}, &prefs.gui_snotice_tabs},       /* Create new tabs for server notices */
	{"gui", "state_save",        CONF_BOOL, {boolv: TRUE},  &prefs.gui_state_save},         /* Save window state on exit */
	{"gui", "style_inputbox",    CONF_BOOL, {boolv: TRUE},  &prefs.gui_style_inputbox},     /* Style the input box a la the main view */
	{"gui", "swap_panes",        CONF_BOOL, {boolv: FALSE}, &prefs.gui_swap_panes},         /* Swap middle & left panes */
	{"gui", "thin_separator",    CONF_BOOL, {boolv: TRUE},  &prefs.gui_thin_separator},     /* Use thin separator */
	{"gui", "throttlemeter",     CONF_INT,  {intv:  2},     &prefs.gui_throttlemeter},      /* Throttlemeter's appearance. 0 = disable, 1 = bar, 2 = text, 3 = both */
	{"gui", "tint_rgb",          CONF_STR,  {strv:  "#C3C3C3"}, &prefs.gui_tint_rgb},       /* Colour to tint the transparent background for the main view */
	{"gui", "transparent",       CONF_BOOL, {boolv: FALSE}, &prefs.gui_transparent},        /* Use transparent background instead of solid colour or image */
	{"gui", "treeview",          CONF_BOOL, {boolv: TRUE},  &prefs.gui_treeview},           /* Use a treeview for listing tabs */
	{"gui", "utility_tabs",      CONF_BOOL, {boolv: FALSE}, &prefs.gui_utility_tabs},       /* Use tabs instead of windows for utilities */
	{"gui", "window_height",     CONF_INT,  {intv:  400},   &prefs.gui_window_height},      /* Default window height */
	{"gui", "window_left",       CONF_INT,  {intv:  0},     &prefs.gui_window_left},        /* Default window Y position */
	{"gui", "window_top",        CONF_INT,  {intv:  0},     &prefs.gui_window_top},         /* Default window X position */
	{"gui", "window_width",      CONF_INT,  {intv:  640},   &prefs.gui_window_width},       /* Default window width */

	/* IRC-specific options */
	{"irc", "bantype",        CONF_INT,  {intv:  0},               &prefs.irc_bantype},         /* Select ban format; 0 = *!*@*.domain, 1 = *!*@host.domain, 2 = *!*user@*.domain; 3 = *!*user@host.domain */
	{"irc", "default_part",   CONF_STR,  {strv: "Leaving."},       &prefs.irc_default_part},    /* Default part message */
	{"irc", "default_quit",   CONF_STR,  {strv: "Ex-Conspirator"}, &prefs.irc_default_quit},    /* Default quit message */
	{"irc", "hide_jpq",       CONF_BOOL, {boolv: FALSE},           &prefs.irc_hide_jpq},        /* Hide joins/parts/quits */
	{"irc", "hide_version",   CONF_BOOL, {boolv: FALSE},           &prefs.irc_hide_version},    /* Ignore VERSION requests */
	{"irc", "nick1",          CONF_STR,  {strv:  ""},              &prefs.irc_nick1},           /* Primary nick */
	{"irc", "nick2",          CONF_STR,  {strv:  ""},              &prefs.irc_nick2},           /* Secondary nick */
	{"irc", "nick3",          CONF_STR,  {strv:  ""},              &prefs.irc_nick3},           /* Tertiary nick */
	{"irc", "real_name",      CONF_STR,  {strv:  ""},              &prefs.irc_real_name},       /* Real name */
	{"irc", "rejoin_on_kick", CONF_BOOL, {boolv: FALSE},           &prefs.irc_rejoin_on_kick},  /* Rejoin channels when kicked */
	{"irc", "skip_motd",      CONF_BOOL, {boolv: FALSE},           &prefs.irc_skip_motd},       /* Don't /motd when connecting */
	{"irc", "user_name",      CONF_STR,  {strv:  ""},              &prefs.irc_user_name},       /* Username */
	{"irc", "user_modes",     CONF_STR,  {strv:  ""},              &prefs.irc_user_modes},      /* Global list of modes to set when connecting to servers */
	{"irc", "who_join",       CONF_BOOL, {boolv: FALSE},           &prefs.irc_who_join},        /* Run WHO upon joining a channel */
	{"irc", "whois_active",   CONF_BOOL, {boolv: TRUE},            &prefs.irc_whois_active},    /* Output WHOIS results in active window */

	/* Loggiong options */
	{"log", "auto",             CONF_BOOL, {boolv: FALSE},      &prefs.log_auto},                           /* Automatically log events */
	{"log", "auto_format",      CONF_STR,  {strv:  "logs/$network/$channel.log"}, &prefs.log_auto_format},  /* Default log filename format */
	{"log", "timestamp",        CONF_BOOL, {boolv: TRUE},       &prefs.log_timestamp},                      /* Timestamp logs */
	{"log", "timestamp_format", CONF_STR,  {strv:  "%H:%M%:S"}, &prefs.log_timestamp_format},               /* Log timestamp format */

	/* Miscellaenous options */
	{"misc", "autosave",      CONF_BOOL, {boolv: TRUE},  &prefs.misc_autosave},       /* Automatically save configuration */
	{"misc", "autosave_url",  CONF_BOOL, {boolv: FALSE}, &prefs.misc_autosave_url},   /* Automatically retain URLs */
	{"misc", "sound_command", CONF_STR,  {strv:  ""},    &prefs.misc_sound_command},  /* Program to run to play custom beep soundfiles with */
	{"misc", "url_modkey",    CONF_INT,  {intv:  4},     &prefs.misc_url_modkey},     /* Modifier key to use for left-clicking URLs; 0 = disable, 1 = shift, 2 = capslock, 3 = ctrl, 4 = alt */

	/* Notify options */
	{"notify", "timeout",      CONF_INT,  {intv:  30},    &prefs.notify_timeout},       /* Check if users are online every n seconds */
	{"notify", "whois_online", CONF_BOOL, {boolv: FALSE}, &prefs.notify_whois_online},  /* WHOIS users on notify */

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

	while (&conf_vt[iter] != NULL) {
		switch (conf_vt[iter].type) {
			case CONF_STR:
				if (!settings_get_string(config, conf_vt[iter].section, conf_vt[iter].key, &strv))
					strv = g_strdup(conf_vt[iter].def.strv);
				conf_vt[iter].value = &strv;
				break;
			case CONF_BOOL:
				if (!settings_get_bool(config, conf_vt[iter].section, conf_vt[iter].key, &boolv))
					memcpy(&boolv, &conf_vt[iter].def.boolv, sizeof(conf_vt[iter].def.boolv));
				conf_vt[iter].value = &boolv;
				break;
			case CONF_INT:
				if (!settings_get_int(config, conf_vt[iter].section, conf_vt[iter].key, &intv))
					memcpy(&intv, &conf_vt[iter].def.intv, sizeof(conf_vt[iter].def.intv));
				conf_vt[iter].value = &intv;
				break;
			case CONF_FLOAT:
				if (!settings_get_float(config, conf_vt[iter].section, conf_vt[iter].key, &floatv))
					memcpy(&floatv, &conf_vt[iter].def.floatv, sizeof(conf_vt[iter].def.floatv));
				conf_vt[iter].value = &floatv;
				break;
			case CONF_DOUBLE:
				if (!settings_get_double(config, conf_vt[iter].section, conf_vt[iter].key, &doublev))
					memcpy(&doublev, &conf_vt[iter].def.doublev, sizeof(conf_vt[iter].def.doublev));
				conf_vt[iter].value = &doublev;
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
	gboolean *boolv;
	gint *intv;
	gfloat *floatv;
	gdouble *doublev;

	while (&conf_vt[iter]) {
		switch (conf_vt[iter].type) {
			case CONF_STR:
				strv = g_strdup(conf_vt[iter].value);
				settings_set_string(config, conf_vt[iter].section, conf_vt[iter].key, strv);
				break;
			case CONF_BOOL:
				boolv = &conf_vt[iter].value;
				settings_set_bool(config, conf_vt[iter].section, conf_vt[iter].key, *boolv);
				break;
			case CONF_INT:
				intv = &conf_vt[iter].value;
				settings_set_int(config, conf_vt[iter].section, conf_vt[iter].key, *intv);
				break;
			case CONF_FLOAT:
				floatv = &conf_vt[iter].value;
				settings_set_float(config, conf_vt[iter].section, conf_vt[iter].key, *floatv);
				break;
			case CONF_DOUBLE:
				doublev = &conf_vt[iter].value;
				settings_set_double(config, conf_vt[iter].section, conf_vt[iter].key, *doublev);
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
