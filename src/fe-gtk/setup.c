/* X-Chat
 * Copyright (C) 2004-2007 Peter Zelezny.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../common/xchat.h"
#include "../common/cfgfiles.h"
#include "../common/fe.h"
#include "../common/text.h"
#include "../common/userlist.h"
#include "../common/util.h"
#include "../common/xchatc.h"
#include "fe-gtk.h"
#include "gtkutil.h"
#include "maingui.h"
#include "palette.h"
#include "pixmaps.h"
#include "menu.h"
#include "tray.h"

#include <gtk/gtkcolorseldialog.h>
#include <gtk/gtktable.h>
#include <gtk/gtkentry.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmisc.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkfontsel.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktreeview.h>
#include <gtk/gtkhbbox.h>
#include <gtk/gtkhseparator.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtktreestore.h>
#include <gtk/gtktreeselection.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkhscale.h>
#include <libsexy/sexy.h>

GtkStyle *create_input_style (GtkStyle *);

#define LABEL_INDENT 12

static int last_selected_page = 0;
static gboolean color_change;
static GtkWidget *cancel_button;

enum
{
	ST_END,
	ST_TOGGLE,
	ST_3OGGLE,
	ST_ENTRY,
	ST_EFONT,
	ST_EFILE,
	ST_EFOLDER,
	ST_MENU,
	ST_RADIO,
	ST_NUMBER,
	ST_HSCALE,
	ST_HEADER,
	ST_LABEL,
	ST_ALERTHEAD
};

typedef struct
{
	int type;
	char *label;
	void *ptr;
	char *tooltip;
	char const *const *list;
	int extra;
} setting;


static const setting textbox_settings[] =
{
	{ST_HEADER,	N_("Text Box Appearance"),0,0,0},
	{ST_EFONT,  N_("Font:"), &prefs.font_normal, 0, 0},
	{ST_EFILE,  N_("Background image:"), &prefs.background, 0, 0},
	{ST_NUMBER,	N_("Scrollback lines:"), &prefs.max_lines,0,0,100000},
	{ST_TOGGLE, N_("Colored nick names"), &prefs.colorednicks,
					N_("Give each person on IRC a different color"),0,0},
	{ST_TOGGLE, N_("Colored nicks (hilighted messages)"), &prefs.coloredhnicks,
		N_("Give each person on IRC a different color for hilighted messages"), 0, 0},
	{ST_TOGGLE, N_("Indent nick names"), &prefs.indent_nicks,
					N_("Make nick names right-justified"),0,0},
	{ST_TOGGLE, N_("Show marker line"), &prefs.show_marker,
					N_("Insert a red line after the last read text."),0,0},
	{ST_TOGGLE, N_("Show redundant nicks"), &prefs.redundant_nickstamps,
		N_("Show nicks when users enter multiple lines of text in a row"), 0, 0},
	{ST_TOGGLE, N_("Automatically replay logged chats"), &prefs.text_replay,
					N_("Automatically replays logs for channels and queries."),0,0},

	{ST_HEADER,	N_("Time Stamps"),0,0,0},
	{ST_TOGGLE, N_("Enable time stamps"), &prefs.timestamp,0,0,2},
	{ST_ENTRY,  N_("Time stamp format:"), &prefs.stamp_format,
					N_("See strftime manpage for details."),0},

	{ST_END, 0, 0, 0, 0, 0}
};

static const char *const tabcompmenu[] =
{
	N_("A-Z"),
	N_("Last-spoke order"),
	NULL
};

static const setting inputbox_settings[] =
{
	{ST_HEADER, N_("Input box"),0,0,0},
	{ST_TOGGLE, N_("Use the Text box font and colors"), &prefs.style_inputbox,0,0,0},
	{ST_TOGGLE, N_("Spell checking"), &prefs.gui_input_spell,0,0,0},
#ifdef REGEX_SUBSTITUTION
	{ST_TOGGLE, N_("Enable regex substitution"), &prefs.text_regex_replace, N_("Use regular expressions on outbound text"), 0, 0},
#endif

	{ST_HEADER, N_("Nick Completion"),0,0,0},
	{ST_TOGGLE, N_("Automatic nick completion (without TAB key)"), &prefs.nickcompletion,
					0,0,0},
	{ST_ENTRY,	N_("Nick completion suffix:"), &prefs.nick_suffix,0,0},
	{ST_MENU,	N_("Nick completion sorted:"), &prefs.completion_sort, 0, tabcompmenu, 0},
	{ST_NUMBER,	N_("Nick completion minimum length:"), &prefs.completion_amount, 0, 0,30},

	{ST_HEADER, N_("Input Box Codes"),0,0,0},
	{ST_TOGGLE, N_("Interpret %nnn as an ASCII value"), &prefs.perc_ascii,0,0,0},
	{ST_TOGGLE, N_("Interpret %C, %B as Color, Bold etc"), &prefs.perc_color,0,0,0},

	{ST_HEADER, N_("Text Overflow"),0,0,0},
	{ST_ENTRY,  N_("Start indicator"), &prefs.text_overflow_start,
		N_("Used to indicate the start of a new message due to overflow."), 0},
	{ST_ENTRY,  N_("Stop indicator"), &prefs.text_overflow_stop,
		N_("Used to indicate the end of any message due to overflow."), 0},

	{ST_END, 0, 0, 0, 0, 0}
};

static const char *const lagmenutext[] =
{
	N_("Off"),
	N_("Graph"),
	N_("Info text"),
	N_("Both"),
	NULL
};

static const char *const ulmenutext[] =
{
	N_("A-Z, Ops first"),
	N_("A-Z"),
	N_("Z-A, Ops last"),
	N_("Z-A"),
	N_("Unsorted"),
	NULL
};

static const char *const cspos[] =
{
	N_("Left (Upper)"),
	N_("Left (Lower)"),
	N_("Right (Upper)"),
	N_("Right (Lower)"),
	N_("Top"),
	N_("Bottom"),
	N_("Hidden"),
	NULL
};

static const char *const ulpos[] =
{
	N_("Left (Upper)"),
	N_("Left (Lower)"),
	N_("Right (Upper)"),
	N_("Right (Lower)"),
	NULL
};

static const setting userlist_settings[] =
{
	{ST_HEADER,	N_("User List"),0,0,0},
	{ST_TOGGLE, N_("Show hostnames in user list"), &prefs.showhostname_in_userlist, 0, 0, 0},
	{ST_TOGGLE, N_("Use the Text box font and colors"), &prefs.style_namelistgad,0,0,0},
/*	{ST_TOGGLE, N_("Resizable user list"), &prefs.paned_userlist,0,0,0},*/
	{ST_MENU,	N_("User list sorted by:"), &prefs.userlist_sort, 0, ulmenutext, 0},
	{ST_MENU,	N_("Show user list at:"), &prefs.gui_ulist_pos, 0, ulpos, 1},

	{ST_HEADER,	N_("Away tracking"),0,0,0},
	{ST_TOGGLE,	N_("Track the Away status of users and mark them in a different color"), &prefs.away_track,0,0,2},
	{ST_NUMBER, N_("On channels smaller than:"), &prefs.away_size_max,0,0,10000},

	{ST_HEADER,	N_("Action Upon Double Click"),0,0,0},
	{ST_ENTRY,	N_("Execute command:"), &prefs.doubleclickuser, 0, 0},

	{ST_HEADER,	N_("Extra Gadgets"),0,0,0},
	{ST_MENU,	N_("Lag meter:"), &prefs.lagometer, 0, lagmenutext, 0},
	{ST_MENU,	N_("Throttle meter:"), &prefs.throttlemeter, 0, lagmenutext, 0},

	{ST_END, 0, 0, 0, 0, 0}
};

static const char *const tabwin[] =
{
	N_("Windows"),
	N_("Tabs"),
	NULL
};

#if 0
static const char *const focusnewtabsmenu[] =
{
	N_("Never"),
	N_("Always"),
	N_("Only requested tabs"),
	NULL
};
#endif

static const setting tabs_settings[] =
{
	/*{ST_HEADER,	N_("Channel Switcher"),0,0,0},*/
	{ST_TOGGLE, N_("Open an extra tab for server messages"), &prefs.use_server_tab, 0, 0, 0},
	{ST_TOGGLE, N_("Open an extra tab for server notices"), &prefs.notices_tabs, 0, 0, 0},
	{ST_TOGGLE, N_("Open a new tab when you receive a private message"), &prefs.autodialog, 0, 0, 0},
	{ST_TOGGLE, N_("Sort tabs in alphabetical order"), &prefs.tab_sort, 0, 0, 0},
	{ST_TOGGLE, N_("Small tabs"), &prefs.tab_small, 0, 0, 0},
#if 0
	{ST_MENU,	N_("Focus new tabs:"), &prefs.newtabstofront, 0, focusnewtabsmenu, 0},
#endif
	{ST_MENU,	N_("Show channel switcher at:"), &prefs.tab_pos, 0, cspos, 1},
	{ST_NUMBER,	N_("Shorten tab labels to:"), &prefs.truncchans, 0, (const char **)N_("letters."), 99},

	{ST_HEADER,	N_("Tabs or Windows"),0,0,0},
	{ST_MENU,	N_("Open channels in:"), &prefs.tabchannels, 0, tabwin, 0},
	{ST_MENU,	N_("Open dialogs in:"), &prefs.privmsgtab, 0, tabwin, 0},
	{ST_MENU,	N_("Open utilities in:"), &prefs.windows_as_tabs, N_("Open DCC, Ignore, Notify etc, in tabs or windows?"), tabwin, 0},

	{ST_END, 0, 0, 0, 0, 0}
};

static const char *const dccaccept[] =
{
	N_("No"),
	N_("Yes"),
	N_("Browse for save folder every time"),
	NULL
};

static const setting filexfer_settings[] =
{
	{ST_HEADER, N_("Files and Directories"), 0, 0, 0},
	{ST_MENU,	N_("Auto accept file offers:"), &prefs.autodccsend, 0, dccaccept, 0},
	{ST_EFOLDER,N_("Download files to:"), &prefs.dccdir, 0, 0},
	{ST_EFOLDER,N_("Move completed files to:"), &prefs.dcc_completed_dir, 0, 0},
	{ST_TOGGLE, N_("Save nick name in filenames"), &prefs.dccwithnick, 0, 0, 0},

	{ST_HEADER,	N_("Auto Open DCC Windows"),0,0,0},
	{ST_TOGGLE, N_("Send window"), &prefs.autoopendccsendwindow, 0, 0, 0},
	{ST_TOGGLE, N_("Receive window"), &prefs.autoopendccrecvwindow, 0, 0, 0},
	{ST_TOGGLE, N_("Chat window"), &prefs.autoopendccchatwindow, 0, 0, 0},

	{ST_HEADER, N_("Network Settings"), 0, 0, 0},
	{ST_TOGGLE, N_("Get my address from the IRC server"), &prefs.ip_from_server,
					N_("Asks the IRC server for your real address. Use this if you have a 192.168.*.* address!"), 0, 0},
	{ST_ENTRY,	N_("DCC IP address:"), &prefs.dcc_ip_str,
					N_("Claim you are at this address when offering files."), 0},
	{ST_NUMBER,	N_("First DCC send port:"), &prefs.first_dcc_send_port, 0, 0, 65535},
	{ST_NUMBER,	N_("Last DCC send port:"), &prefs.last_dcc_send_port, 0,
		(const char **)N_("!Leave ports at zero for full range."), 65535},

	{ST_HEADER, N_("Maximum File Transfer Speeds (bytes per second)"), 0, 0, 0},
	{ST_NUMBER,	N_("One upload:"), &prefs.dcc_max_send_cps,
					N_("Maximum speed for one transfer"), 0, 1000000},
	{ST_NUMBER,	N_("One download:"), &prefs.dcc_max_get_cps,
					N_("Maximum speed for one transfer"), 0, 1000000},
	{ST_NUMBER,	N_("All uploads combined:"), &prefs.dcc_global_max_send_cps,
					N_("Maximum speed for all files"), 0, 1000000},
	{ST_NUMBER,	N_("All downloads combined:"), &prefs.dcc_global_max_get_cps,
					N_("Maximum speed for all files"), 0, 1000000},

	{ST_END, 0, 0, 0, 0, 0}
};

static void * balloonlist[3] =
{
	&prefs.input_balloon_chans, &prefs.input_balloon_priv, &prefs.input_balloon_hilight
};

static void * trayblinklist[3] =
{
	&prefs.input_tray_chans, &prefs.input_tray_priv, &prefs.input_tray_hilight
};

static void * taskbarlist[3] =
{
	&prefs.input_flash_chans, &prefs.input_flash_priv, &prefs.input_flash_hilight
};

static void * beeplist[3] =
{
	&prefs.input_beep_chans, &prefs.input_beep_priv, &prefs.input_beep_hilight
};

static const setting alert_settings[] =
{
	{ST_HEADER,	N_("Alerts"),0,0,0},

	{ST_ALERTHEAD},
	{ST_3OGGLE, N_("Show tray balloons on:"), 0, 0, (void *)balloonlist, 0},
	{ST_3OGGLE, N_("Blink tray icon on:"), 0, 0, (void *)trayblinklist, 0},
	{ST_3OGGLE, N_("Blink task bar on:"), 0, 0, (void *)taskbarlist, 0},
	{ST_3OGGLE, N_("Make a beep sound on:"), 0, 0, (void *)beeplist, 0},

	{ST_TOGGLE,	N_("Enable system tray icon"), &prefs.gui_tray, 0, 0, 0},

	{ST_HEADER,	N_("Highlighted Messages"),0,0,0},
	{ST_LABEL,	N_("Highlighted messages are ones where your nickname is mentioned, but also:"), 0, 0, 0, 1},

	{ST_ENTRY,	N_("Extra words to highlight:"), &prefs.irc_extra_hilight, 0, 0},
	{ST_ENTRY,	N_("Nick names not to highlight:"), &prefs.irc_no_hilight, 0, 0},
	{ST_ENTRY,	N_("Nick names to always highlight:"), &prefs.irc_nick_hilight, 0, 0},
	{ST_LABEL,	N_("Separate multiple words with commas.")},
	{ST_END, 0, 0, 0, 0, 0}
};

static const setting general_settings[] =
{
	{ST_HEADER,	N_("Default Messages"),0,0,0},
	{ST_ENTRY,	N_("Quit:"), &prefs.quitreason, 0, 0},
	{ST_ENTRY,	N_("Leave channel:"), &prefs.partreason, 0, 0},
	{ST_ENTRY,	N_("Away:"), &prefs.awayreason, 0, 0},

	{ST_HEADER,	N_("Away"),0,0,0},
	{ST_TOGGLE,	N_("Announce away messages"), &prefs.show_away_message,
					N_("Announce your away messages to all channels"), 0, 0},
	{ST_TOGGLE,	N_("Show away once"), &prefs.show_away_once, N_("Show identical away messages only once"), 0, 0},
	{ST_TOGGLE,	N_("Automatically unmark away"), &prefs.auto_unmark_away, N_("Unmark yourself as away before sending messages"), 0, 0},

	{ST_HEADER,	N_("Other Features"), 0, 0, 0},
	{ST_TOGGLE,	N_("Strip IRCd quit messages"), &prefs.strip_quits,
					N_("Strip quit prefixes like \"Quit:\" on IRC systems which use them."), 0, 0},
	{ST_END, 0, 0, 0, 0, 0}
};

static const setting advanced_settings[] =
{
	{ST_HEADER,	N_("Advanced Settings"),0,0,0},
	{ST_NUMBER,	N_("Auto reconnect delay:"), &prefs.recon_delay, 0, 0, 9999},
	{ST_TOGGLE,	N_("Whois on notify"), &prefs.whois_on_notifyonline, N_("Sends a /WHOIS when a user comes online in your notify list"), 0, 0},
	{ST_TOGGLE,	N_("Hide join and part messages"), &prefs.confmode, N_("Hide channel join/part messages by default"), 0, 0},

	{ST_END, 0, 0, 0, 0, 0}
};

static const setting logging_settings[] =
{
	{ST_HEADER,	N_("Logging"),0,0,0},
	{ST_TOGGLE,	N_("Enable logging of conversations"), &prefs.logging, 0, 0, 2},
	{ST_ENTRY,	N_("Log filename:"), &prefs.logmask, 0, 0},
	{ST_LABEL,	N_("%s=Server %c=Channel %n=Network.")},

	{ST_HEADER,	N_("Time Stamps"),0,0,0},
	{ST_TOGGLE,	N_("Insert timestamps in logs"), &prefs.timestamp_logs, 0, 0, 2},
	{ST_ENTRY,	N_("Log timestamp format:"), &prefs.timestamp_log_format, 0, 0},
	{ST_LABEL,	N_("See strftime manpage for details.")},

	{ST_END, 0, 0, 0, 0, 0}
};

static const char *const proxytypes[] =
{
	N_("(Disabled)"),
	N_("Wingate"),
	N_("Socks4"),
	N_("Socks5"),
	N_("HTTP"),
#ifdef USE_MSPROXY
	N_("MS Proxy (ISA)"),
#endif
	NULL
};

static const char *const proxyuse[] =
{
	N_("All Connections"),
	N_("IRC Server Only"),
	N_("DCC Get Only"),
	NULL
};

static const setting network_settings[] =
{
	{ST_HEADER,	N_("Your Address"), 0, 0, 0, 0},
	{ST_ENTRY,	N_("Bind to:"), &prefs.hostname, 0, 0},
	{ST_LABEL,	N_("Only useful for computers with multiple addresses.")},

	{ST_HEADER,	N_("Proxy Server"), 0, 0, 0, 0},
	{ST_ENTRY,	N_("Hostname:"), &prefs.proxy_host, 0, 0},
	{ST_NUMBER,	N_("Port:"), &prefs.proxy_port, 0, 0, 65535},
	{ST_MENU,	N_("Type:"), &prefs.proxy_type, 0, proxytypes, 0},
	{ST_MENU,	N_("Use proxy for:"), &prefs.proxy_use, 0, proxyuse, 0},

	{ST_HEADER,	N_("Proxy Authentication"), 0, 0, 0, 0},
#ifdef USE_MSPROXY
	{ST_TOGGLE,	N_("Use Authentication (MS Proxy, HTTP or Socks5 only)"), &prefs.proxy_auth, 0, 0, 0},
#else
	{ST_TOGGLE,	N_("Use Authentication (HTTP or Socks5 only)"), &prefs.proxy_auth, 0, 0, 0},
#endif
	{ST_ENTRY,	N_("Username:"), &prefs.proxy_user, 0, 0},
	{ST_ENTRY,	N_("Password:"), &prefs.proxy_pass, 0, GINT_TO_POINTER(1)},

	{ST_END, 0, 0, 0, 0, 0}
};

static const setting gui_colors_gtk[] =
{
	{ST_TOGGLE, N_("Use colors derived from GTK theme"), &prefs.gtk_colors, 0, 0, 0},
	{ST_END, 0, 0, 0, 0, 0}
};

#define setup_get_int(pr,set) *(((int *)pr)+set->offset)
#define setup_get_int3(pr,off) *(((int *)pr)+off)

#define setup_set_int(pr,set,num) *((int *)pr+set->offset)=num

static void
setup_3oggle_cb (GtkToggleButton *but, unsigned int *setting)
{
	*setting = but->active;
}

static void
setup_headlabel (GtkWidget *tab, int row, int col, char *text)
{
	GtkWidget *label;
	char buf[128];
	char *sp;

	snprintf (buf, sizeof (buf), "<b><span size=\"smaller\">%s</span></b>", text);
	sp = strchr (buf + 17, ' ');
	if (sp)
		*sp = '\n';

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), buf);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (tab), label, col, col + 1, row, row + 1, 0, 0, 4, 0);
}

static void
setup_create_alert_header (GtkWidget *tab, int row, const setting *set)
{
	setup_headlabel (tab, row, 3, _("Channel Message"));
	setup_headlabel (tab, row, 4, _("Private Message"));
	setup_headlabel (tab, row, 5, _("Highlighted Message"));
}

/* makes 3 toggles side-by-side */

static void
setup_create_3oggle (GtkWidget *tab, int row, const setting *set)
{
	GtkWidget *label, *wid;
	int **offsets = (int **)set->list;

	label = gtk_label_new (_(set->label));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (tab), label, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	wid = gtk_check_button_new ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), *offsets[0]);
	g_signal_connect (G_OBJECT (wid), "toggled",
							G_CALLBACK (setup_3oggle_cb), offsets[0]);
	gtk_table_attach (GTK_TABLE (tab), wid, 3, 4, row, row + 1, 0, 0, 0, 0);

	wid = gtk_check_button_new ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), *offsets[1]);
	g_signal_connect (G_OBJECT (wid), "toggled",
							G_CALLBACK (setup_3oggle_cb), offsets[1]);
	gtk_table_attach (GTK_TABLE (tab), wid, 4, 5, row, row + 1, 0, 0, 0, 0);

	wid = gtk_check_button_new ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), *offsets[2]);
	g_signal_connect (G_OBJECT (wid), "toggled",
							G_CALLBACK (setup_3oggle_cb), offsets[2]);
	gtk_table_attach (GTK_TABLE (tab), wid, 5, 6, row, row + 1, 0, 0, 0, 0);
}

static void
setup_toggle_cb (GtkToggleButton *but, const setting *set)
{
	GtkWidget *label, *disable_wid;

	*((int *) set->ptr) = but->active ? 1 : 0;

	/* does this toggle also enable/disable another widget? */
	disable_wid = g_object_get_data (G_OBJECT (but), "nxt");
	if (disable_wid)
	{
		gtk_widget_set_sensitive (disable_wid, but->active);
		label = g_object_get_data (G_OBJECT (disable_wid), "lbl");
		gtk_widget_set_sensitive (label, but->active);
	}
}

static GtkWidget *
setup_create_toggle (GtkWidget *tab, int row, const setting *set)
{
	GtkWidget *wid;

	wid = gtk_check_button_new_with_label (_(set->label));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), *((int *) set->ptr));
	g_signal_connect (G_OBJECT (wid), "toggled",
							G_CALLBACK (setup_toggle_cb), (gpointer)set);
	if (set->tooltip)
		add_tip (wid, _(set->tooltip));
	gtk_table_attach (GTK_TABLE (tab), wid, 2, row==6 ? 6 : 4, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	return wid;
}

static GtkWidget *
setup_create_italic_label (char *text)
{
	GtkWidget *label;
	char buf[256];

	label = gtk_label_new (NULL);
	snprintf (buf, sizeof (buf), "<i><span size=\"smaller\">%s</span></i>", text);
	gtk_label_set_markup (GTK_LABEL (label), buf);

	return label;
}

static void
setup_spin_cb (GtkSpinButton *spin, const setting *set)
{
	*((int *) set->ptr) = gtk_spin_button_get_value_as_int(spin);
}

static GtkWidget *
setup_create_spin (GtkWidget *table, int row, const setting *set)
{
	GtkWidget *label, *wid, *rbox, *align;
	char *text;

	label = gtk_label_new (_(set->label));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	align = gtk_alignment_new (0.0, 0.5, 0.0, 0.0);
	gtk_table_attach (GTK_TABLE (table), align, 3, 4, row, row + 1,
							GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

	rbox = gtk_hbox_new (0, 0);
	gtk_container_add (GTK_CONTAINER (align), rbox);

	wid = gtk_spin_button_new_with_range (0, set->extra, 1);
	g_object_set_data (G_OBJECT (wid), "lbl", label);
	if (set->tooltip)
		add_tip (wid, _(set->tooltip));
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (wid), *((int *) set->ptr));
	g_signal_connect (G_OBJECT (wid), "value_changed",
							G_CALLBACK (setup_spin_cb), (gpointer)set);
	gtk_box_pack_start (GTK_BOX (rbox), wid, 0, 0, 0);

	if (set->list)
	{
		text = _((char *)set->list);
		if (text[0] == '!')
			label = setup_create_italic_label (text + 1);
		else
			label = gtk_label_new (text);
		gtk_box_pack_start (GTK_BOX (rbox), label, 0, 0, 6);
	}

	return wid;
}

static void
setup_hscale_cb (GtkHScale *wid, const setting *set)
{
	*((int *) set->ptr) = gtk_range_get_value(GTK_RANGE(wid));
}

static void
setup_create_hscale (GtkWidget *table, int row, const setting *set)
{
	GtkWidget *wid;

	wid = gtk_label_new (_(set->label));
	gtk_misc_set_alignment (GTK_MISC (wid), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), wid, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	wid = gtk_hscale_new_with_range (0., 255., 1.);
	gtk_scale_set_value_pos (GTK_SCALE (wid), GTK_POS_RIGHT);
	gtk_range_set_value (GTK_RANGE (wid), *((int *) set->ptr));
	g_signal_connect (G_OBJECT(wid), "value_changed",
							G_CALLBACK (setup_hscale_cb), (gpointer)set);
	gtk_table_attach (GTK_TABLE (table), wid, 3, 6, row, row + 1,
							GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
}


static GtkWidget *proxy_user; 	/* username GtkEntry */
static GtkWidget *proxy_pass; 	/* password GtkEntry */

static void
setup_menu_cb (GtkWidget *cbox, const setting *set)
{
	int n = gtk_combo_box_get_active (GTK_COMBO_BOX (cbox));

	/* set the prefs.<field> */
	*((int *) set->ptr) = n + set->extra;

	if (set->list == proxytypes)
	{
		/* only HTTP and Socks5 can use a username/pass */
		gtk_widget_set_sensitive (proxy_user, (n == 3 || n == 4 || n == 5));

		gtk_widget_set_sensitive (proxy_pass, (n == 3 || n == 4 || n == 5));
	}
}

static void
setup_radio_cb (GtkWidget *item, const setting *set)
{
	if (GTK_TOGGLE_BUTTON (item)->active)
	{
		int n = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (item), "n"));

		/* set the prefs.<field> */
		*((int *) set->ptr) = n;
	}
}

static int
setup_create_radio (GtkWidget *table, int row, const setting *set)
{
	GtkWidget *wid, *hbox;
	int i;
	const char **text = (const char **)set->list;
	GSList *group;

	wid = gtk_label_new (_(set->label));
	gtk_misc_set_alignment (GTK_MISC (wid), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), wid, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	hbox = gtk_hbox_new (0, 0);
	gtk_table_attach (GTK_TABLE (table), hbox, 3, 4, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

	i = 0;
	group = NULL;
	while (text[i])
	{
		if (text[i][0] != 0)
		{
			wid = gtk_radio_button_new_with_mnemonic (group, text[i]);
			/*if (set->tooltip)
				add_tip (wid, _(set->tooltip));*/
			group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (wid));
			gtk_container_add (GTK_CONTAINER (hbox), wid);
			if (i == *((int *) set->ptr))
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), TRUE);
			g_object_set_data (G_OBJECT (wid), "n", GINT_TO_POINTER (i));
			g_signal_connect (G_OBJECT (wid), "toggled",
									G_CALLBACK (setup_radio_cb), (gpointer)set);
		}
		i++;
		row++;
	}

	return i;
}

/*
static const char *id_strings[] =
{
	"",
	"*",
	"%C4*%C18%B%B",
	"%U"
};

static void
setup_id_menu_cb (GtkWidget *item, char *dest)
{
	int n = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (item), "n"));

	strcpy (dest, id_strings[n]);
}

static void
setup_create_id_menu (GtkWidget *table, char *label, int row, char *dest)
{
	GtkWidget *wid, *menu, *item;
	int i, def = 0;
	static const char *text[] =
	{
		("(disabled)"),
		("A star (*)"),
		("A red star (*)"),
		("Underlined")
	};

	wid = gtk_label_new (label);
	gtk_misc_set_alignment (GTK_MISC (wid), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), wid, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	wid = gtk_option_menu_new ();
	menu = gtk_menu_new ();

	for (i = 0; i < 4; i++)
	{
		if (strcmp (id_strings[i], dest) == 0)
		{
			def = i;
			break;
		}
	}

	i = 0;
	while (text[i])
	{
		item = gtk_menu_item_new_with_label (_(text[i]));
		g_object_set_data (G_OBJECT (item), "n", GINT_TO_POINTER (i));

		gtk_widget_show (item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		g_signal_connect (G_OBJECT (item), "activate",
								G_CALLBACK (setup_id_menu_cb), dest);
		i++;
	}

	gtk_option_menu_set_menu (GTK_OPTION_MENU (wid), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (wid), def);

	gtk_table_attach (GTK_TABLE (table), wid, 3, 4, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
}

*/

static void
setup_create_menu (GtkWidget *table, int row, const setting *set)
{
	GtkWidget *wid, *cbox, *box;
	const char **text = (const char **)set->list;
	int i;

	wid = gtk_label_new (_(set->label));
	gtk_misc_set_alignment (GTK_MISC (wid), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), wid, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	cbox = gtk_combo_box_new_text ();

	for (i = 0; text[i]; i++)
		gtk_combo_box_append_text (GTK_COMBO_BOX (cbox), _(text[i]));

	gtk_combo_box_set_active (GTK_COMBO_BOX (cbox), *((int *) set->ptr) - set->extra);
	g_signal_connect (G_OBJECT (cbox), "changed",
							G_CALLBACK (setup_menu_cb), (gpointer)set);

	box = gtk_hbox_new (0, 0);
	gtk_box_pack_start (GTK_BOX (box), cbox, 0, 0, 0);
	gtk_table_attach (GTK_TABLE (table), box, 3, 4, row, row + 1,
							GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
}

static void
setup_filereq_cb (GtkWidget *entry, char *file)
{
	if (file)
	{
		if (file[0])
			gtk_entry_set_text (GTK_ENTRY (entry), file);
	}
}

static void
setup_browsefile_cb (GtkWidget *button, GtkWidget *entry)
{
	gtkutil_file_req (_("Select an Image File"), setup_filereq_cb, entry, NULL, 0);
}

static void
setup_fontsel_cb (GtkWidget *button, GtkFontSelectionDialog *dialog)
{
	GtkWidget *entry;

	entry = g_object_get_data (G_OBJECT (button), "e");

	gtk_entry_set_text (GTK_ENTRY (entry),
							  gtk_font_selection_dialog_get_font_name (dialog));
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
setup_fontsel_cancel (GtkWidget *button, GtkFontSelectionDialog *dialog)
{
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
setup_browsefolder_cb (GtkWidget *button, GtkEntry *entry)
{
	gtkutil_file_req (_("Select Download Folder"), setup_filereq_cb, entry, entry->text, FRF_CHOOSEFOLDER);
}

static void
setup_browsefont_cb (GtkWidget *button, GtkWidget *entry)
{
	GtkFontSelection *sel;
	GtkFontSelectionDialog *dialog;

	dialog = (GtkFontSelectionDialog *) gtk_font_selection_dialog_new (_("Select font"));

	sel = (GtkFontSelection *) dialog->fontsel;

	if (GTK_ENTRY (entry)->text[0])
		gtk_font_selection_set_font_name (sel, GTK_ENTRY (entry)->text);

	g_object_set_data (G_OBJECT (dialog->ok_button), "e", entry);

	g_signal_connect (G_OBJECT (dialog->ok_button), "clicked",
							G_CALLBACK (setup_fontsel_cb), dialog);
	g_signal_connect (G_OBJECT (dialog->cancel_button), "clicked",
							G_CALLBACK (setup_fontsel_cancel), dialog);

	gtk_widget_show (GTK_WIDGET (dialog));
}

static void
setup_entry_cb (GtkEntry *entry, setting *set)
{
	/* atomically set the new setting, so that it's thread safe. --nenolod */
	char *p = g_strdup(entry->text);
	char *pp = *((char **) set->ptr);

	*((char **) set->ptr) = p;
	g_free(pp);
}

static void
setup_create_label (GtkWidget *table, int row, const setting *set)
{
	gtk_table_attach (GTK_TABLE (table), setup_create_italic_label (_(set->label)),
							set->extra ? 1 : 3, 5, row, row + 1, GTK_FILL,
							GTK_SHRINK | GTK_FILL, 0, 0);
}

static GtkWidget *
setup_create_entry (GtkWidget *table, int row, const setting *set)
{
	GtkWidget *label;
	GtkWidget *wid, *bwid;

	label = gtk_label_new (_(set->label));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	wid = gtk_entry_new ();
	g_object_set_data (G_OBJECT (wid), "lbl", label);
	if (set->list)
		gtk_entry_set_visibility (GTK_ENTRY (wid), FALSE);
	if (set->tooltip)
		add_tip (wid, _(set->tooltip));
	gtk_entry_set_max_length (GTK_ENTRY (wid), set->extra - 1);

	if (*((char **) set->ptr) != NULL)
		gtk_entry_set_text (GTK_ENTRY (wid), *((char **) set->ptr));

	g_signal_connect (G_OBJECT (wid), "changed",
							G_CALLBACK (setup_entry_cb), (gpointer)set);

	if (set->ptr == &prefs.proxy_user)
		proxy_user = wid;
	if (set->ptr == &prefs.proxy_pass)
		proxy_pass = wid;

	/* only http and Socks5 can auth */
	if ( (set->ptr == &prefs.proxy_pass ||
			set->ptr == &prefs.proxy_user) &&
	     (prefs.proxy_type != 4 && prefs.proxy_type != 3 && prefs.proxy_type != 5) )
		gtk_widget_set_sensitive (wid, FALSE);

	if (set->type == ST_ENTRY)
		gtk_table_attach (GTK_TABLE (table), wid, 3, 6, row, row + 1,
								GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	else
	{
		gtk_table_attach (GTK_TABLE (table), wid, 3, 5, row, row + 1,
								GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
		bwid = gtk_button_new_with_label (_("Browse..."));
		gtk_table_attach (GTK_TABLE (table), bwid, 5, 6, row, row + 1,
								GTK_SHRINK | GTK_FILL, GTK_FILL, 0, 0);
		if (set->type == ST_EFILE)
			g_signal_connect (G_OBJECT (bwid), "clicked",
									G_CALLBACK (setup_browsefile_cb), wid);
		if (set->type == ST_EFONT)
			g_signal_connect (G_OBJECT (bwid), "clicked",
									G_CALLBACK (setup_browsefont_cb), wid);
		if (set->type == ST_EFOLDER)
			g_signal_connect (G_OBJECT (bwid), "clicked",
									G_CALLBACK (setup_browsefolder_cb), wid);
	}

	return wid;
}

static void
setup_create_header (GtkWidget *table, int row, char *labeltext)
{
	GtkWidget *label;
	char buf[128];

	if (row == 0)
		snprintf (buf, sizeof (buf), "<b>%s</b>", _(labeltext));
	else
		snprintf (buf, sizeof (buf), "\n<b>%s</b>", _(labeltext));

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), buf);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 4, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 5);
}

static GtkWidget *
setup_create_frame (GtkWidget **left, GtkWidget *box)
{
	GtkWidget *tab, *hbox, *inbox = box;

	tab = gtk_table_new (3, 2, FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (tab), 6);
	gtk_table_set_row_spacings (GTK_TABLE (tab), 2);
	gtk_table_set_col_spacings (GTK_TABLE (tab), 3);
	gtk_container_add (GTK_CONTAINER (inbox), tab);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (inbox), hbox);

	*left = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), *left, 0, 0, 0);

	return tab;
}

static void
open_data_cb (GtkWidget *button, gpointer data)
{
	fe_open_url (get_xdir_utf8 ());
}

static GtkWidget *
setup_create_page (const setting *set)
{
	int i, row, do_disable;
	GtkWidget *tab, *box, *left;
	GtkWidget *wid = NULL, *prev;

	box = gtk_vbox_new (FALSE, 1);
	gtk_container_set_border_width (GTK_CONTAINER (box), 6);

	tab = setup_create_frame (&left, box);

	i = row = do_disable = 0;
	while (set[i].type != ST_END)
	{
		prev = wid;

		switch (set[i].type)
		{
		case ST_HEADER:
			setup_create_header (tab, row, set[i].label);
			break;
		case ST_EFONT:
		case ST_ENTRY:
		case ST_EFILE:
		case ST_EFOLDER:
			wid = setup_create_entry (tab, row, &set[i]);
			break;
		case ST_TOGGLE:
			wid = setup_create_toggle (tab, row, &set[i]);
			do_disable = set[i].extra;
			break;
		case ST_3OGGLE:
			setup_create_3oggle (tab, row, &set[i]);
			break;
		case ST_MENU:
			setup_create_menu (tab, row, &set[i]);
			break;
		case ST_RADIO:
			row += setup_create_radio (tab, row, &set[i]);
			break;
		case ST_NUMBER:
			wid = setup_create_spin (tab, row, &set[i]);
			break;
		case ST_HSCALE:
			setup_create_hscale (tab, row, &set[i]);
			break;
		case ST_LABEL:
			setup_create_label (tab, row, &set[i]);
			break;
		case ST_ALERTHEAD:
			setup_create_alert_header (tab, row, &set[i]);
		}

		/* will this toggle disable the "next" widget? */
		do_disable--;
		if (do_disable == 0)
		{
			/* setup_toggle_cb uses this data */
			g_object_set_data (G_OBJECT (prev), "nxt", wid);
			/* force initial sensitive state */
			gtk_widget_set_sensitive (wid, GTK_TOGGLE_BUTTON (prev)->active);
			gtk_widget_set_sensitive (g_object_get_data (G_OBJECT (wid), "lbl"),
											  GTK_TOGGLE_BUTTON (prev)->active);
		}

		i++;
		row++;
	}

	if (set == logging_settings)
	{
		GtkWidget *but = gtk_button_new_with_label (_("Open Data Folder"));
		gtk_box_pack_start (GTK_BOX (left), but, 0, 0, 0);
		g_signal_connect (G_OBJECT (but), "clicked",
								G_CALLBACK (open_data_cb), 0);
	}

	return box;
}

static void
setup_color_ok_cb (GtkWidget *button, GtkWidget *dialog)
{
	GtkColorSelectionDialog *cdialog = GTK_COLOR_SELECTION_DIALOG (dialog);
	GdkColor *col;
	GdkColor old_color;
	GtkStyle *style;

	col = g_object_get_data (G_OBJECT (button), "c");
	old_color = *col;

	button = g_object_get_data (G_OBJECT (button), "b");

	if (!GTK_IS_WIDGET (button))
	{
		gtk_widget_destroy (dialog);
		return;
	}

	color_change = TRUE;

	gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (cdialog->colorsel), col);

	gdk_colormap_alloc_color (gtk_widget_get_colormap (button), col, TRUE, TRUE);

	style = gtk_style_new ();
	style->bg[0] = *col;
	gtk_widget_set_style (button, style);
	g_object_unref (style);

	/* is this line correct?? */
	gdk_colormap_free_colors (gtk_widget_get_colormap (button), &old_color, 1);

	gtk_widget_destroy (dialog);
}

static void
setup_color_cb (GtkWidget *button, gpointer userdata)
{
	GtkWidget *dialog;
	GtkColorSelectionDialog *cdialog;
	GdkColor *color;

	color = &colors[GPOINTER_TO_INT (userdata)];

	dialog = gtk_color_selection_dialog_new (_("Select color"));
	cdialog = GTK_COLOR_SELECTION_DIALOG (dialog);

	gtk_widget_hide (cdialog->help_button);
	g_signal_connect (G_OBJECT (cdialog->ok_button), "clicked",
							G_CALLBACK (setup_color_ok_cb), dialog);
	g_signal_connect (G_OBJECT (cdialog->cancel_button), "clicked",
							G_CALLBACK (gtkutil_destroy), dialog);
	g_object_set_data (G_OBJECT (cdialog->ok_button), "c", color);
	g_object_set_data (G_OBJECT (cdialog->ok_button), "b", button);
	gtk_widget_set_sensitive (cdialog->help_button, FALSE);
	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (cdialog->colorsel), color);
	gtk_widget_show (dialog);
}

static void
setup_create_color_button (GtkWidget *table, int num, int row, int col)
{
	GtkWidget *but;
	GtkStyle *style;
	char buf[64];

	if (num > 31)
		strcpy (buf, "<span size=\"x-small\"> </span>");
	else
						/* 12345678901 23456789 01  23456789 */
		sprintf (buf, "<span size=\"x-small\">%d</span>", num);
	but = gtk_button_new_with_label (" ");
	gtk_label_set_markup (GTK_LABEL (GTK_BIN (but)->child), buf);
	/* win32 build uses this to turn off themeing */
	g_object_set_data (G_OBJECT (but), "xchat-color", (gpointer)1);
	gtk_table_attach (GTK_TABLE (table), but, col, col+1, row, row+1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
	g_signal_connect (G_OBJECT (but), "clicked",
							G_CALLBACK (setup_color_cb), GINT_TO_POINTER (num));
	style = gtk_style_new ();
	style->bg[GTK_STATE_NORMAL] = colors[num];
	gtk_widget_set_style (but, style);
	g_object_unref (style);
}

static void
setup_create_other_colorR (char *text, int num, int row, GtkWidget *tab)
{
	GtkWidget *label;

	label = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (tab), label, 5, 9, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);
	setup_create_color_button (tab, num, row, 9);
}

static void
setup_create_other_color (char *text, int num, int row, GtkWidget *tab)
{
	GtkWidget *label;

	label = gtk_label_new (text);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (tab), label, 2, 3, row, row + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);
	setup_create_color_button (tab, num, row, 3);
}

static GtkWidget *
setup_create_color_page (void)
{
	GtkWidget *tab, *box, *label, *wid;
	int i;

	box = gtk_vbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), 6);

	tab = gtk_table_new (9, 2, FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (tab), 6);
	gtk_table_set_row_spacings (GTK_TABLE (tab), 2);
	gtk_table_set_col_spacings (GTK_TABLE (tab), 3);
	gtk_container_add (GTK_CONTAINER (box), tab);

	setup_create_header (tab, 0, N_("Text Colors"));

	label = gtk_label_new (_("mIRC colors:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (tab), label, 2, 3, 1, 2,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	for (i = 0; i < 16; i++)
		setup_create_color_button (tab, i, 1, i+3);

	label = gtk_label_new (_("Local colors:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (tab), label, 2, 3, 2, 3,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	for (i = 16; i < 32; i++)
		setup_create_color_button (tab, i, 2, (i+3) - 16);

	setup_create_other_color (_("Foreground:"), COL_FG, 3, tab);
	setup_create_other_colorR (_("Background:"), COL_BG, 3, tab);

	setup_create_header (tab, 5, N_("Marking Text"));

	setup_create_other_color (_("Foreground:"), COL_MARK_FG, 6, tab);
	setup_create_other_colorR (_("Background:"), COL_MARK_BG, 6, tab);

	setup_create_header (tab, 8, N_("Interface Colors"));

	setup_create_other_color (_("New data:"), COL_NEW_DATA, 9, tab);
	setup_create_other_colorR (_("Marker line:"), COL_MARKER, 9, tab);
	setup_create_other_color (_("New message:"), COL_NEW_MSG, 10, tab);
	setup_create_other_colorR (_("Away user:"), COL_AWAY, 10, tab);
	setup_create_other_color (_("Highlight:"), COL_HILIGHT, 11, tab);

	setup_create_header(tab, 15, N_("Color options:"));

	wid = gtk_check_button_new_with_label (_(gui_colors_gtk[0].label));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wid), *((int *) gui_colors_gtk[0].ptr));
	g_signal_connect (G_OBJECT (wid), "toggled",
							G_CALLBACK (setup_toggle_cb), (gpointer)&gui_colors_gtk[0]);
	gtk_table_attach (GTK_TABLE (tab), wid, 2, 9, 16, 16 + 1,
							GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, LABEL_INDENT, 0);

	return box;
}

static void
setup_add_page (const char *title, GtkWidget *book, GtkWidget *tab)
{
	GtkWidget *oframe, *frame, *label, *vvbox;
	char buf[128];

	/* frame for whole page */
	oframe = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (oframe), GTK_SHADOW_IN);

	vvbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (oframe), vvbox);

	/* border for the label */
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	gtk_box_pack_start (GTK_BOX (vvbox), frame, FALSE, TRUE, 0);

	/* label */
	label = gtk_label_new (NULL);
	snprintf (buf, sizeof (buf), "<b><big>%s</big></b>", _(title));
	gtk_label_set_markup (GTK_LABEL (label), buf);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 2, 1);
	gtk_container_add (GTK_CONTAINER (frame), label);

	gtk_container_add (GTK_CONTAINER (vvbox), tab);

	gtk_notebook_append_page (GTK_NOTEBOOK (book), oframe, NULL);
}

static const char *const cata[] =
{
	N_("Interface"),
		N_("Text box"),
		N_("Input box"),
		N_("User list"),
		N_("Channel switcher"),
		N_("Colors"),
		NULL,
	N_("Chatting"),
		N_("Alerts"),
		N_("General"),
		N_("Logging"),
		N_("Advanced"),
		NULL,
	N_("Network"),
		N_("Network setup"),
		N_("File transfers"),
		NULL,
	NULL
};

static GtkWidget *
setup_create_pages (GtkWidget *box)
{
	GtkWidget *book;

	book = gtk_notebook_new ();

	setup_add_page (cata[1], book, setup_create_page (textbox_settings));
	setup_add_page (cata[2], book, setup_create_page (inputbox_settings));
	setup_add_page (cata[3], book, setup_create_page (userlist_settings));
	setup_add_page (cata[4], book, setup_create_page (tabs_settings));
	setup_add_page (cata[5], book, setup_create_color_page ());
	setup_add_page (cata[8], book, setup_create_page (alert_settings));
	setup_add_page (cata[9], book, setup_create_page (general_settings));
	setup_add_page (cata[10], book, setup_create_page (logging_settings));
	setup_add_page (cata[11], book, setup_create_page(advanced_settings));
	setup_add_page (cata[14], book, setup_create_page (network_settings));
	setup_add_page (cata[15], book, setup_create_page (filexfer_settings));

	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (book), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (book), FALSE);
	gtk_container_add (GTK_CONTAINER (box), book);

	return book;
}

static void
setup_tree_cb (GtkTreeView *treeview, GtkWidget *book)
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview);
	GtkTreeIter iter;
	GtkTreeModel *model;
	int page;

	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 1, &page, -1);
		if (page != -1)
		{
			gtk_notebook_set_current_page (GTK_NOTEBOOK (book), page);
			last_selected_page = page;
		}
	}
}

static gboolean
setup_tree_select_filter (GtkTreeSelection *selection, GtkTreeModel *model,
								  GtkTreePath *path, gboolean path_selected,
								  gpointer data)
{
	if (gtk_tree_path_get_depth (path) > 1)
		return TRUE;
	return FALSE;
}

static void
setup_create_tree (GtkWidget *box, GtkWidget *book)
{
	GtkWidget *tree;
	GtkWidget *frame;
	GtkTreeStore *model;
	GtkTreeIter iter;
	GtkTreeIter child_iter;
	GtkTreeIter *sel_iter = NULL;
	GtkCellRenderer *renderer;
	GtkTreeSelection *sel;
	int i, page;

	model = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_INT);

	i = 0;
	page = 0;
	do
	{
		gtk_tree_store_append (model, &iter, NULL);
		gtk_tree_store_set (model, &iter, 0, _(cata[i]), 1, -1, -1);
		i++;

		do
		{
			gtk_tree_store_append (model, &child_iter, &iter);
			gtk_tree_store_set (model, &child_iter, 0, _(cata[i]), 1, page, -1);
			if (page == last_selected_page)
				sel_iter = gtk_tree_iter_copy (&child_iter);
			page++;
			i++;
		} while (cata[i]);

		i++;

	} while (cata[i]);

	tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	g_object_unref (G_OBJECT (model));
	sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	gtk_tree_selection_set_mode (sel, GTK_SELECTION_BROWSE);
	gtk_tree_selection_set_select_function (sel, setup_tree_select_filter,
														 NULL, NULL);
	g_signal_connect (G_OBJECT (tree), "cursor_changed",
							G_CALLBACK (setup_tree_cb), book);

	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (tree),
							    -1, _("Categories"), renderer, "text", 0, NULL);
	gtk_tree_view_expand_all (GTK_TREE_VIEW (tree));

	frame = gtk_frame_new (NULL);
	gtk_container_add (GTK_CONTAINER (frame), tree);
	gtk_box_pack_start (GTK_BOX (box), frame, 0, 0, 0);
	gtk_box_reorder_child (GTK_BOX (box), frame, 0);

	if (sel_iter)
	{
		gtk_tree_selection_select_iter (sel, sel_iter);
		gtk_tree_iter_free (sel_iter);
	}
}

static void
setup_apply_entry_style (GtkWidget *entry)
{
	gtk_widget_modify_base (entry, GTK_STATE_NORMAL, &colors[COL_BG]);
	gtk_widget_modify_text (entry, GTK_STATE_NORMAL, &colors[COL_FG]);
	gtk_widget_modify_font (entry, input_style->font_desc);
}

static void
setup_apply_to_sess (session_gui *gui)
{
	mg_update_xtext (gui->xtext);

	if (prefs.style_namelistgad)
		gtk_widget_set_style (gui->user_tree, input_style);

	if (prefs.style_inputbox)
	{
		extern char cursor_color_rc[];
		char buf[256];
		sprintf (buf, cursor_color_rc,
				(colors[COL_FG].red >> 8),
				(colors[COL_FG].green >> 8),
				(colors[COL_FG].blue >> 8));
		gtk_rc_parse_string (buf);

		setup_apply_entry_style (gui->input_box);
		setup_apply_entry_style (gui->limit_entry);
		setup_apply_entry_style (gui->key_entry);
		setup_apply_entry_style (gui->topic_entry);
	}

	if (prefs.userlistbuttons)
		gtk_widget_show (gui->button_box);
	else
		gtk_widget_hide (gui->button_box);

	sexy_spell_entry_set_checked ((SexySpellEntry *)gui->input_box, prefs.gui_input_spell);
}

static void
unslash (char *dir)
{
	if (dir && dir[0])
	{
		int len = strlen (dir) - 1;
		if (dir[len] == '/')
			dir[len] = 0;
	}
}

void
setup_apply_real (int new_pix, int do_ulist, int do_layout)
{
	GSList *list;
	session *sess;
	int done_main = FALSE;

	/* remove trailing slashes */
	unslash (prefs.dccdir);
	unslash (prefs.dcc_completed_dir);

	mkdir_utf8 (prefs.dccdir);
	mkdir_utf8 (prefs.dcc_completed_dir);

	if (new_pix)
	{
		if (channelwin_pix)
			g_object_unref (channelwin_pix);
		channelwin_pix = pixmap_load_from_file (prefs.background);
	}

	input_style = create_input_style (input_style);

	list = sess_list;
	while (list)
	{
		sess = list->data;
		if (sess->gui->is_tab)
		{
			/* only apply to main tabwindow once */
			if (!done_main)
			{
				done_main = TRUE;
				setup_apply_to_sess (sess->gui);
			}
		} else
		{
			setup_apply_to_sess (sess->gui);
		}

		if (prefs.logging)
			log_open (sess);
		else
			log_close (sess);

		if (do_ulist)
			userlist_rehash (sess);

		list = list->next;
	}

	mg_apply_setup ();
	tray_apply_setup ();
}

static void
setup_apply (struct xchatprefs *pr)
{
	int new_pix = FALSE;
	int noapply = FALSE;
	int do_ulist = FALSE;
	int do_layout = FALSE;

	if ((!pr->background && prefs.background) || (pr->background && !prefs.background) ||
		(pr->background && prefs.background && strcmp(pr->background, prefs.background) != 0))
		new_pix = TRUE;

#define DIFF(a) (pr->a != prefs.a)

	if (DIFF (paned_userlist))
		noapply = TRUE;
	if (DIFF (lagometer))
		noapply = TRUE;
	if (DIFF (throttlemeter))
		noapply = TRUE;
	if (DIFF (showhostname_in_userlist))
		noapply = TRUE;
	if (DIFF (tab_small))
		noapply = TRUE;
	if (DIFF (tab_sort))
		noapply = TRUE;
	if (DIFF (use_server_tab))
		noapply = TRUE;
	if (DIFF (style_namelistgad))
		noapply = TRUE;
	if (DIFF (truncchans))
		noapply = TRUE;
	if (DIFF (tab_layout))
		do_layout = TRUE;

	if (color_change || (DIFF (away_size_max)) || (DIFF (away_track)))
		do_ulist = TRUE;

	if ((pr->tab_pos == 5 || pr->tab_pos == 6) &&
		 pr->tab_layout == 2 && pr->tab_pos != prefs.tab_pos)
		fe_message (_("You cannot place the tree on the top or bottom!\n"
						"Please change to the <b>Tabs</b> layout in the <b>View</b>"
						" menu first."),
						FE_MSG_WARN | FE_MSG_MARKUP);

	memcpy (&prefs, pr, sizeof (prefs));

	setup_apply_real (new_pix, do_ulist, do_layout);

	if (noapply)
		fe_message (_("Some settings were changed that require a"
						" restart to take full effect."), FE_MSG_WARN);

	if (prefs.autodccsend == 1)
	{
		if (!strcmp ((char *)g_get_home_dir (), prefs.dccdir))
		{
			fe_message (_("*WARNING*\n"
							 "Auto accepting DCC to your home directory\n"
							 "can be dangerous and is exploitable. Eg:\n"
							 "Someone could send you a .bash_profile"), FE_MSG_WARN);
		}
	}
}

static void
setup_ok_cb (GtkWidget *but, GtkWidget *win)
{
	gtk_widget_destroy (win);
	setup_apply (&prefs);
	save_config ();
	palette_save ();
}

static GtkWidget *
setup_window_open (void)
{
	GtkWidget *wid, *win, *vbox, *hbox, *hbbox;

	win = gtkutil_window_new (_("conspire: Preferences"), "prefs", 0, 0, 3);

	vbox = gtk_vbox_new (FALSE, 5);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
	gtk_container_add (GTK_CONTAINER (win), vbox);

	hbox = gtk_hbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (vbox), hbox);

	setup_create_tree (hbox, setup_create_pages (hbox));

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	/* prepare the button box */
	hbbox = gtk_hbutton_box_new ();
	gtk_box_set_spacing (GTK_BOX (hbbox), 4);
	gtk_box_pack_end (GTK_BOX (hbox), hbbox, FALSE, FALSE, 0);

	/* standard buttons */
	/* GNOME doesn't like apply */
#if 0
	wid = gtk_button_new_from_stock (GTK_STOCK_APPLY);
	g_signal_connect (G_OBJECT (wid), "clicked",
							G_CALLBACK (setup_apply_cb), win);
	gtk_box_pack_start (GTK_BOX (hbbox), wid, FALSE, FALSE, 0);
#endif

	cancel_button = wid = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
	g_signal_connect (G_OBJECT (wid), "clicked",
							G_CALLBACK (gtkutil_destroy), win);
	gtk_box_pack_start (GTK_BOX (hbbox), wid, FALSE, FALSE, 0);

	wid = gtk_button_new_from_stock (GTK_STOCK_OK);
	g_signal_connect (G_OBJECT (wid), "clicked",
							G_CALLBACK (setup_ok_cb), win);
	gtk_box_pack_start (GTK_BOX (hbbox), wid, FALSE, FALSE, 0);

	wid = gtk_hseparator_new ();
	gtk_box_pack_end (GTK_BOX (vbox), wid, FALSE, FALSE, 0);

	gtk_widget_show_all (win);

	return win;
}

static void
setup_close_cb (GtkWidget *win, GtkWidget **swin)
{
	*swin = NULL;
}

void
setup_open (void)
{
	static GtkWidget *setup_window = NULL;

	if (setup_window)
	{
		gtk_window_present (GTK_WINDOW (setup_window));
		return;
	}

	color_change = FALSE;
	setup_window = setup_window_open ();

	g_signal_connect (G_OBJECT (setup_window), "destroy",
							G_CALLBACK (setup_close_cb), &setup_window);
}
