from history cimport *
from cap cimport *
from linequeue cimport *
from signal_factory cimport *

IF GNUTLS:
    cdef from extern "gnutls/gnutls.h":
        struct gnutls_session_t:
            pass
        struct gnutls_certificate_credentials_t:
            pass
        ctypedef struct gnutls_session_t gnutls_session_t
        ctypedef struct gnutls_certificate_credentials_t gnutls_certificate_credentials_t

cdef from extern "time.h":
    ctypedef __time_t time_t

cdef from extern "xchat.h":
    struct msproxy_state_t:
	int clientid;
	int serverid;
	unsigned char seq_recv
	unsigned char seq_sent

    enum SaslState:
        SASL_INITIALIZED = 0
        SASL_AUTHENTICATING
        SASL_COMPLETE

    struct server:
        int port
        int sok
        int sok4
        int sok6
        int proxy_sok
        int proxy_sok4
        int proxy_sok6
        struct msproxy_state_t msp_state
        int id
        IF GNUTLS:
            gnutls_session_t gnutls_session
            gnutls_certificate_credentials_t gnutls_x509cred

        int childread
        int childwrite
        int childpid
        int iotag
        int recondelay_tag
        int joindelay_tag
        char hostname[128]
        char servername[128]
        char password[86]
        char nick[NICKLEN]
        char linebuf[2048]
        char *last_away_reason
        int pos
        int nickcount
        int nickservtype

        char *chantypes
        char *chanmodes
        char *nick_prefixes
        char *nick_modes
        char *bad_nick_prefixes
        int modes_per_line

        void *network

        int lag

        guint split_timer
        char *split_reason
        char *split_serv1
        char *split_serv2

        struct session *front_session
        struct session *server_session

        struct server_gui *gui

        unsigned int ctcp_counter
        time_t ctcp_last_time

        unsigned int msg_counter
        time_t msg_last_time

        time_t lag_sent
        time_t ping_recv
        time_t away_time

        char *encoding

        int motd_skipped
        unsigned int connected
        unsigned int connecting
        int no_login
        int skip_next_userhost
        int inside_whois
        int doing_dns
        unsigned int end_of_motd
        int sent_quit
        int use_listargs
        unsigned int is_away
        int reconnect_away
        int dont_use_proxy
        int supports_watch
        int supports_monitor
        int inside_monitor
        int bad_prefix
        unsigned int have_namesx
        unsigned int have_uhnames
        unsigned int have_whox
        unsigned int have_capab
        unsigned int have_idmsg
        unsigned int have_except
        unsigned int using_cp1255
        unsigned int using_irc
        int use_who
        
        IF GNUTLS:
            int use_ssl
            int accept_invalid_cert

        char *sasl_user
        char *sasl_pass
        SaslState sasl_state
        int sasl_timeout_tag
        CapState *cap
        LineQueue *lq

    struct session:
        struct server *server
        void *usertree
        void *usertree_alpha
        struct User *me
        char channel[CHANLEN]
        char waitchannel[CHANLEN]
        char willjoinchannel[CHANLEN]
        char channelkey[64]
        int limit
        int logfd
        int scrollfd
        int scrollwritten

        char lastnick[NICKLEN]
        GSList *split_list

        struct history history

        int ops
        int hops
        int voices
        int total

        char *quitreason
        char *topic
        char *current_modes

        int mode_timeout_tag

        struct session *lastlog_sess

        struct session_gui *gui
        struct restore_gui *res

        int userlisthidden
        int type
        GList *lastact_elem
        int lastact_idx

        int new_data
        int nick_said
        int msg_said
        int ignore_date
        int ignore_mode
        int ignore_names
        int end_of_names
        int doing_who

        unsigned int hide_join_part
        unsigned int beep
        unsigned int tray
        unsigned int color_paste
        int done_away_check
        unsigned int lastlog_regexp
        char immutable
        char ul_blocked

    struct xchatprefs:
        char *nick1
        char *nick2
        char *nick3
        char *realname
        char *username
        char *nick_suffix
        char *awayreason
        char *quitreason
        char *partreason
        char *font_normal
        char *doubleclickuser
        char *background
        char *dccdir
        char *dcc_completed_dir
        char *irc_extra_hilight
        char *irc_no_hilight
        char *irc_nick_hilight
        char *dnsprogram
        char *hostname
        char *cmdchar
        char *logmask
        char *stamp_format
        char *timestamp_log_format
        char *irc_id_ytext
        char *irc_id_ntext
        char *irc_time_format

        char *text_overflow_start
        char *text_overflow_stop

        char *proxy_host
        int proxy_port
        int proxy_type
        int proxy_use
        unsigned int proxy_auth
        char *proxy_user
        char *proxy_pass

        int first_dcc_send_port
        int last_dcc_send_port

        int tint_red
        int tint_green
        int tint_blue

        int away_timeout
        int away_size_max

        int gui_pane_left_size
        int gui_pane_right_size

        int gui_ulist_pos
        int tab_pos

        int _tabs_position
        int tab_layout
        int max_auto_indent
        int dcc_blocksize
        int max_lines
        int notify_timeout
        int dcctimeout
        int dccstalltimeout
        int dcc_global_max_get_cps
        int dcc_global_max_send_cps
        int dcc_max_get_cps
        int dcc_max_send_cps
        int mainwindow_left
        int mainwindow_top
        int mainwindow_width
        int mainwindow_height
        int completion_sort
        int gui_win_state
        int gui_url_mod
        int gui_usermenu
        int gui_join_dialog
        int gui_quit_dialog
        int dialog_left
        int dialog_top
        int dialog_width
        int dialog_height
        int dccpermissions
        int recon_delay
        int bantype
        int userlist_sort
        int local_ip
        int dcc_ip
        char *dcc_ip_str

        unsigned int tab_small
        unsigned int tab_sort
        unsigned int tab_icons
        unsigned int mainwindow_save
        unsigned int perc_color
        unsigned int perc_ascii
        unsigned int autosave
        unsigned int autodialog
        unsigned int gtk_colors
        unsigned int autosave_url
        unsigned int autoreconnect
        unsigned int autoreconnectonfail
        unsigned int invisible
        unsigned int servernotice
        unsigned int wallops
        unsigned int skipmotd
        unsigned int autorejoin
        unsigned int colorednicks
        unsigned int coloredhnicks
        unsigned int chanmodebuttons
        unsigned int userlistbuttons
        unsigned int showhostname_in_userlist
        unsigned int nickcompletion
        unsigned int completion_amount
        unsigned int tabchannels
        unsigned int paned_userlist
        unsigned int autodccchat
        unsigned int autodccsend
        unsigned int autoresume
        unsigned int autoopendccsendwindow
        unsigned int autoopendccrecvwindow
        unsigned int autoopendccchatwindow
        unsigned int transparent
        unsigned int stripcolor
        unsigned int timestamp
        unsigned int fastdccsend
        unsigned int dcc_send_fillspaces
        unsigned int dcc_remove
        unsigned int slist_select
        unsigned int filterbeep

        unsigned int input_balloon_chans
        unsigned int input_balloon_hilight
        unsigned int input_balloon_priv

        unsigned int input_beep_chans
        unsigned int input_beep_hilight
        unsigned int input_beep_priv

        unsigned int input_flash_chans
        unsigned int input_flash_hilight
        unsigned int input_flash_priv

        unsigned int input_tray_chans
        unsigned int input_tray_hilight
        unsigned int input_tray_priv

        unsigned int truncchans
        unsigned int privmsgtab
        unsigned int irc_join_delay
        unsigned int logging
        unsigned int timestamp_logs
        unsigned int newtabstofront
        unsigned int dccwithnick
        unsigned int hidever
        unsigned int ip_from_server
        unsigned int show_away_once
        unsigned int show_away_message
        unsigned int auto_unmark_away
        unsigned int away_track
        unsigned int userhost
        unsigned int use_server_tab
        unsigned int notices_tabs
        unsigned int style_namelistgad
        unsigned int style_inputbox
        unsigned int windows_as_tabs
        unsigned int indent_nicks
        unsigned int text_replay
        unsigned int show_marker
        unsigned int show_separator
        unsigned int thin_separator
        unsigned int auto_indent
        unsigned int wordwrap
        unsigned int gui_input_spell
        unsigned int gui_tray
        unsigned int gui_tray_flags
        unsigned int gui_tweaks
        unsigned int _gui_ulist_left
        unsigned int throttle
        unsigned int topicbar
        unsigned int hideuserlist
        unsigned int hidemenu
        unsigned int perlwarnings
        unsigned int lagometer
        unsigned int throttlemeter
        unsigned int pingtimeout
        unsigned int whois_on_notifyonline
        unsigned int wait_on_exit
        unsigned int confmode
        unsigned int utf8_locale
        unsigned int identd
        unsigned int skip_serverlist
        IF REGEX_SUBSTITUTION:
            unsigned int text_regex_replace

        unsigned int ctcp_number_limit
        unsigned int ctcp_time_limit

        unsigned int msg_number_limit
        unsigned int msg_time_limit

        unsigned int save_pevents

        char redundant_nickstamps
        char strip_quits
        char hilight_enable

    struct popup:
        char *cmd
        char *name

    struct regex_entry:
        char *cmd
        char *name
        GRegex *regex

