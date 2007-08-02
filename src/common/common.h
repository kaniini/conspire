#ifndef COMMON_H
#define COMMON_H

#include <glib.h>

extern int xchat_is_quitting;
extern gint arg_skip_plugins;	/* command-line args */
extern gint arg_dont_autoconnect;
extern char *arg_url;
extern gint arg_existing;

extern session *current_sess;
extern session *current_tab;

extern GSList *popup_list;
extern GSList *button_list;
extern GSList *dlgbutton_list;
extern GSList *command_list;
extern GSList *ctcp_list;
extern GSList *replace_list;
extern GSList *sess_list;
extern GSList *dcc_list;
extern GSList *ignore_list;
extern GSList *usermenu_list;
extern GSList *urlhandler_list;
extern GSList *tabmenu_list;

#endif /* COMMON_H */

