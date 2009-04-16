/* Copyright (C) 2006-2007 Peter Zelezny. */

#include <string.h>
#include <unistd.h>
#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/inbound.h"
#include "../common/server.h"
#include "../common/fe.h"
#include "../common/util.h"
#include "fe-gtk.h"
#include "pixmaps.h"
#include "maingui.h"
#include "menu.h"
#include "tray.h"

#include <gtk/gtk.h>
#include <libnotify/notify.h>

typedef enum	/* current icon status */
{
	TS_NONE,
	TS_MESSAGE,
	TS_HIGHLIGHT,
	TS_FILEOFFER,
	TS_CUSTOM /* plugin */
} TrayStatus;

typedef enum
{
	WS_FOCUSED,
	WS_NORMAL,
	WS_HIDDEN
} WinStatus;

typedef GdkPixbuf* TrayIcon;
#define tray_icon_from_file(f) gdk_pixbuf_new_from_file(f,NULL)
#define tray_icon_free(i) g_object_unref(i)

#define ICON_NORMAL pix_conspire
#define ICON_MSG pix_tray_blank
#define ICON_HILIGHT pix_tray_blank
#define ICON_FILE pix_tray_file
#define TIMEOUT 500

static GtkStatusIcon *sticon;
static gint flash_tag;
static TrayStatus tray_status;

static TrayIcon custom_icon1;
static TrayIcon custom_icon2;

static int tray_priv_count = 0;
static int tray_pub_count = 0;
static int tray_hilight_count = 0;
static int tray_invite_count = 0;
static int tray_dcc_count = 0;

static WinStatus
tray_get_window_status(void)
{
	session *sess = sess_list->data;

	switch (fe_gui_info(sess, 0))
	{
		case 0:
			return WS_NORMAL;
		case 1:
			return WS_FOCUSED;
		case 2:
		default:
			return WS_HIDDEN;
	}
}

static int
tray_count_channels (void)
{
	int cons = 0;
	GSList *list;
	session *sess;

	for (list = sess_list; list; list = list->next)
	{
		sess = list->data;
		if (sess->server->connected && sess->channel[0] &&
			 sess->type == SESS_CHANNEL)
			cons++;
	}
	return cons;
}

static int
tray_count_networks (void)
{
	int cons = 0;
	GSList *list;

	for (list = serv_list; list; list = list->next)
	{
		if (((server *)list->data)->connected)
			cons++;
	}
	return cons;
}

void
fe_tray_set_tooltip(const char *text)
{
	if (sticon)
		gtk_status_icon_set_tooltip (sticon, text);
}

void
fe_tray_set_balloon(const char *title, const char *text)
{
	char *stext;
	WinStatus ws;
	NotifyNotification *n;

	/* no balloons if the window is focused */
	ws = tray_get_window_status();
	if (ws == WS_FOCUSED)
		return;

	/* bit 1 of flags means "no balloons unless hidden/iconified" */
	if (ws != WS_HIDDEN && (prefs.gui_tray_flags & 2))
		return;

	/* FIXME: this should close the current balloon */
	if (!text)
		return;

	stext = strip_color(text, -1, STRIP_ALL);
	n = notify_notification_new(title, stext, NULL, NULL);
	notify_notification_attach_to_status_icon(n, sticon);
	notify_notification_set_timeout(n, 20000);
	notify_notification_show(n, NULL);

	free(stext);
	g_object_unref(G_OBJECT(n));
}

static void
tray_set_balloonf(const char *text, const char *format, ...)
{
	va_list args;
	char *buf;

	va_start (args, format);
	buf = g_strdup_vprintf (format, args);
	va_end (args);

	fe_tray_set_balloon(buf, text);
	g_free (buf);
}

static void
tray_set_tipf(const char *format, ...)
{
	va_list args;
	char *buf;

	va_start (args, format);
	buf = g_strdup_vprintf (format, args);
	va_end (args);

	fe_tray_set_tooltip(buf);
	g_free (buf);
}

static void
tray_stop_flash(void)
{
	int nets, chans;

	if (flash_tag)
	{
		g_source_remove (flash_tag);
		flash_tag = 0;
	}

	if (sticon)
	{
		gtk_status_icon_set_from_pixbuf (sticon, ICON_NORMAL);
		nets = tray_count_networks ();
		chans = tray_count_channels ();
		if (nets)
			tray_set_tipf(_("conspire: Connected to %u networks and %u channels"),
								nets, chans);
		else
			tray_set_tipf("conspire: %s", _("Not connected."));
	}

	if (custom_icon1)
	{
		tray_icon_free (custom_icon1);
		custom_icon1 = NULL;
	}

	if (custom_icon2)
	{
		tray_icon_free (custom_icon2);
		custom_icon2 = NULL;
	}

	tray_status = TS_NONE;
}

static void
tray_reset_counts (void)
{
	tray_priv_count = 0;
	tray_pub_count = 0;
	tray_hilight_count = 0;
	tray_invite_count = 0;
	tray_dcc_count = 0;
}

static int
tray_timeout_cb (TrayIcon icon)
{
	if (custom_icon1)
	{
		if (gtk_status_icon_get_pixbuf (sticon) == custom_icon1)
		{
			if (custom_icon2)
				gtk_status_icon_set_from_pixbuf (sticon, custom_icon2);
			else
				gtk_status_icon_set_from_pixbuf (sticon, ICON_NORMAL);
		}
		else
		{
			gtk_status_icon_set_from_pixbuf (sticon, custom_icon1);
		}
	}
	else if (icon != ICON_FILE)
	{
		if (gtk_status_icon_get_pixbuf (sticon) == ICON_NORMAL)
			gtk_status_icon_set_from_pixbuf (sticon, icon);
		else
			gtk_status_icon_set_from_pixbuf (sticon, ICON_NORMAL);
	}
	else if (icon == ICON_FILE)
	{
		if (gtk_status_icon_get_pixbuf (sticon) == ICON_FILE)
			gtk_status_icon_set_from_pixbuf (sticon, ICON_MSG);
		else
			gtk_status_icon_set_from_pixbuf (sticon, icon);
	}
	return 1;
}

static void
tray_set_flash(TrayIcon icon)
{
	/* don't ever flash the window if it's focused, that's just dumb. --nenolod */
	if (tray_get_window_status() == WS_FOCUSED)
		return;

	if (!sticon)
		return;

	/* already flashing the same icon */
	if (flash_tag && gtk_status_icon_get_pixbuf (sticon) == icon)
		return;

	/* no flashing if window is focused */
	if (tray_get_window_status() == WS_FOCUSED)
		return;

	tray_stop_flash();

	gtk_status_icon_set_from_pixbuf (sticon, icon);
	flash_tag = g_timeout_add (TIMEOUT, (GSourceFunc) tray_timeout_cb, icon);
}

void
fe_tray_set_flash(const char *filename1, const char *filename2, int tout)
{
	tray_apply_setup();
	if (!sticon)
		return;

	tray_stop_flash();

	if (tout == -1)
		tout = TIMEOUT;

	custom_icon1 = tray_icon_from_file (filename1);
	if (filename2)
		custom_icon2 = tray_icon_from_file (filename2);

	gtk_status_icon_set_from_pixbuf (sticon, custom_icon1);
	flash_tag = g_timeout_add (tout, (GSourceFunc) tray_timeout_cb, NULL);
	tray_status = TS_CUSTOM;
}

void
fe_tray_set_icon(feicon icon)
{
	tray_apply_setup();
	if (!sticon)
		return;

	tray_stop_flash();

	switch (icon)
	{
	case FE_ICON_NORMAL:
		break;
	case FE_ICON_MESSAGE:
		tray_set_flash(ICON_MSG);
		break;
	case FE_ICON_HIGHLIGHT:
	case FE_ICON_PRIVMSG:
		tray_set_flash(ICON_HILIGHT);
		break;
	case FE_ICON_FILEOFFER:
		tray_set_flash(ICON_FILE);
	}
}

void
fe_tray_set_file(const char *filename)
{
	tray_apply_setup();
	if (!sticon)
		return;

	tray_stop_flash();

	if (filename)
	{
		custom_icon1 = tray_icon_from_file (filename);
		gtk_status_icon_set_from_pixbuf (sticon, custom_icon1);
		tray_status = TS_CUSTOM;
	}
}

gboolean
tray_toggle_visibility(gboolean force_hide)
{
	static int x, y;
	static GdkScreen *screen;
	GtkWindow *win;
	session *sess = sess_list->data;

	if (!sticon)
		return FALSE;

	win = fe_gui_info_ptr(sess, 0);

	tray_stop_flash();
	tray_reset_counts();

	if (!win)
		return FALSE;

	if (force_hide || GTK_WIDGET_VISIBLE(win))
	{
		gtk_window_get_position(win, &x, &y);
		screen = gtk_window_get_screen(win);
		gtk_widget_hide(GTK_WIDGET(win));
	}
	else
	{
		gtk_window_set_screen(win, screen);
		gtk_window_move(win, x, y);
		gtk_widget_show(GTK_WIDGET (win));
		gtk_window_present(win);
	}

	return TRUE;
}

static void
tray_menu_restore_cb(GtkWidget *item, gpointer userdata)
{
	tray_toggle_visibility (FALSE);
}

static void
tray_menu_quit_cb(GtkWidget *item, gpointer userdata)
{
	mg_quit();
}

static void
tray_make_item (GtkWidget *menu, char *label, void *callback, void *userdata)
{
	GtkWidget *item;

	if (label)
		item = gtk_menu_item_new_with_mnemonic (label);
	else
		item = gtk_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	g_signal_connect (G_OBJECT (item), "activate",
							G_CALLBACK (callback), userdata);
	gtk_widget_show (item);
}

static void
tray_toggle_cb(GtkCheckMenuItem *item, unsigned int *setting)
{
	*setting = item->active;
}

static void
blink_item(unsigned int *setting, GtkWidget *menu, char *label)
{
	menu_toggle_item (label, menu, tray_toggle_cb, setting, *setting);
}

static void
tray_menu_destroy(GtkWidget *menu, gpointer userdata)
{
	gtk_widget_destroy (menu);
	g_object_unref (menu);
}

static void
tray_menu_cb(GtkWidget *widget, guint button, guint time, gpointer userdata)
{
	GtkWidget *menu;
	GtkWidget *submenu;

	menu = gtk_menu_new ();
	/*gtk_menu_set_screen (GTK_MENU (menu), gtk_widget_get_screen (widget));*/

	if (tray_get_window_status() == WS_HIDDEN)
		tray_make_item (menu, _("_Restore"), tray_menu_restore_cb, NULL);
	else
		tray_make_item (menu, _("_Hide"), tray_menu_restore_cb, NULL);
	tray_make_item (menu, NULL, tray_menu_quit_cb, NULL);

	submenu = mg_submenu (menu, _("_Blink on"));
	blink_item (&prefs.input_tray_chans, submenu, _("Channel Message"));
	blink_item (&prefs.input_tray_priv, submenu, _("Private Message"));
	blink_item (&prefs.input_tray_hilight, submenu, _("Highlighted Message"));
	/*blink_item (BIT_FILEOFFER, submenu, _("File Offer"));*/

	tray_make_item (menu, NULL, tray_menu_quit_cb, NULL);
	mg_create_icon_item (_("_Quit"), GTK_STOCK_QUIT, menu, tray_menu_quit_cb, NULL);

	menu_add_plugin_items (menu, "\x5$TRAY", NULL);

	g_object_ref (menu);
	g_object_ref_sink (menu);
	g_object_unref (menu);
	g_signal_connect (G_OBJECT (menu), "selection-done",
							G_CALLBACK (tray_menu_destroy), NULL);

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, gtk_status_icon_position_menu,
						 userdata, button, time);
}

static void
tray_ui_show()
{
	flash_tag = 0;
	tray_status = TS_NONE;
	custom_icon1 = NULL;
	custom_icon2 = NULL;

	sticon = gtk_status_icon_new_from_pixbuf (ICON_NORMAL);
	if (!sticon)
		return;

	g_signal_connect (G_OBJECT (sticon), "popup-menu",
							G_CALLBACK (tray_menu_cb), sticon);
	g_signal_connect (G_OBJECT (sticon), "activate",
							G_CALLBACK (tray_menu_restore_cb), NULL);
}

static void
process_message_highlight(gpointer *params)
{
	session *sess   = params[0];
	gchar *from     = params[1];
	gchar *message  = params[2];

	if (prefs.input_tray_hilight)
	{
		tray_set_flash(ICON_HILIGHT);

		/* FIXME: hides any previous private messages */
		tray_hilight_count++;
		if (tray_hilight_count == 1)
			tray_set_tipf(_("conspire: Highlighted message from: %s (%s)"),
								from, sess->channel ? sess->channel : _("unknown channel"));
		else
			tray_set_tipf(_("conspire: %u highlighted messages, latest from: %s (%s)"),
								tray_hilight_count, from, sess->channel ? sess->channel : _("unknown channel"));
	}

	if (prefs.input_balloon_hilight)
		tray_set_balloonf(message, _("conspire: Highlighted message from: %s (%s)"),
								 from, sess->channel ? sess->channel : _("unknown channel"));
}

static void
process_message(gpointer *params)
{
	session *sess  = params[0];
	gchar *from    = params[1];
	gchar *message = params[2];

	if (tray_status == TS_HIGHLIGHT)
		return;

	if (prefs.input_tray_chans)
	{
		tray_set_flash(ICON_MSG);

		tray_pub_count++;
		if (tray_pub_count == 1)
			tray_set_tipf(_("conspire: New public message from: %s (%s)"),
								from, sess->channel ? sess->channel : _("unknown channel"));
		else
			tray_set_tipf(_("conspire: %u new public messages."), tray_pub_count);
	}

	if (prefs.input_balloon_chans)
		tray_set_balloonf(message, _("conspire: New public message from: %s (%s)"),
								 from, sess->channel ? sess->channel : _("unknown channel"));
}

static void
process_private(gpointer *params)
{
	session *sess  = params[0];
	gchar *from    = params[1];
	gchar *message = params[2];

	const char *network = server_get_network(sess->server, FALSE);
	if (!network)
		network = sess->server->connected ? sess->server->servername : NULL;

	if (FromNick(from, prefs.irc_no_hilight))
		return;

	if (prefs.input_tray_priv)
	{
		tray_set_flash(ICON_HILIGHT);

		tray_priv_count++;
		if (tray_priv_count == 1)
			tray_set_tipf(_("conspire: Private message from: %s (%s)"),
								from, network);
		else
			tray_set_tipf(_("conspire: %u private messages, latest from: %s (%s)"),
								tray_priv_count, from, network);
	}

	if (prefs.input_balloon_priv)
		tray_set_balloonf(message, _("conspire: Private message from: %s (%s)"),
								 from, network);
}

static void
process_invited(gpointer *params)
{
	gchar **word  = params[1];
	gchar *from   = params[2];
	server *serv  = params[3];

	gchar *channel = word[4][0] == ':' ? word[4] + 1 : word[4];

	const char *network = server_get_network(serv, FALSE);
	if (!network)
		network = serv->connected ? serv->servername : NULL;

	if (FromNick(from, prefs.irc_no_hilight))
		return;

	if (prefs.input_tray_priv)
	{
		tray_set_flash(ICON_HILIGHT);

		tray_invite_count++;
		if (tray_invite_count == 1)
			tray_set_tipf(_("conspire: Invite from: %s (%s) to %s"),
								from, network, channel);
		else
			tray_set_tipf(_("conspire: %u private messages, latest from: %s (%s) to %s"),
								tray_priv_count, from, network, channel);
	}

	if (prefs.input_balloon_priv)
		tray_set_balloonf("", _("conspire: Invite from: %s (%s) to %s"),
								 from, network, channel);
}

static void
process_dcc(gpointer *params)
{
	session *sess = params[0];
	gchar *nick   = params[1];

	const char *network = server_get_network(sess->server, FALSE);
	if (!network)
		network = sess->server->connected ? sess->server->servername : NULL;

	if (prefs.input_tray_priv)
	{
		tray_set_flash(ICON_FILE);

		tray_dcc_count++;
		if (tray_dcc_count == 1)
			tray_set_tipf(_("conspire: DCC offer from: %s (%s)"),
								nick, network ? network : "unknown network");
		else
			tray_set_tipf(_("conspire: %u DCC offers, latest from: %s (%s)"),
								tray_dcc_count, nick, network ? network : "unknown network");
	}

	if (prefs.input_balloon_priv)
		tray_set_balloonf("", _("conspire: DCC offer from: %s (%s)"),
								nick, network ? network : "unknown network");
}

static void
tray_focus_cb(gpointer *unused)
{
	tray_stop_flash();
	tray_reset_counts();
}

static void
tray_ui_hide()
{
	tray_stop_flash();

	if (sticon)
	{
		g_object_unref ((GObject *)sticon);
		sticon = NULL;
	}
}

void
tray_apply_setup(void)
{
	if (sticon)
	{
		if (!prefs.gui_tray)
			tray_ui_hide();
	}
	else
	{
		if (prefs.gui_tray)
			tray_ui_show();
	}
}

void
tray_init(void)
{
	signal_attach("action public hilight", process_message_highlight);
	signal_attach("message public hilight", process_message_highlight);

	signal_attach("action public", process_message);
	signal_attach("message public", process_message);
	signal_attach("notice public", process_message);

	signal_attach("message private", process_private);
	signal_attach("notice private", process_private);

	signal_attach("channel invited", process_invited);

	signal_attach("dcc file request", process_dcc);
	signal_attach("dcc chat request", process_dcc);
	signal_attach("dcc generic offer", process_dcc);

	signal_attach("gui focused", tray_focus_cb);

	notify_init(PACKAGE_NAME);
}
