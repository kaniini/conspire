#ifndef CONSPIRE_IGNORE_H
#define CONSPIRE_IGNORE_H

enum {
    IGNORE_NONE    = 0x00000000,
    IGNORE_PRIVATE = 0x00000001,
    IGNORE_PUBLIC  = 0x00000002,
    IGNORE_NOTICE  = 0x00000004,
    IGNORE_SNOTES  = 0x00000008,
    IGNORE_CTCP    = 0x00000010,
    IGNORE_ACTION  = 0x00000020,
    IGNORE_JOINS   = 0x00000040,
    IGNORE_PARTS   = 0x00000080,
    IGNORE_QUITS   = 0x00000100,
    IGNORE_KICKS   = 0x00000200,
    IGNORE_MODES   = 0x00000400,
    IGNORE_TOPICS  = 0x00000800,
    IGNORE_INVITES = 0x00001000,
    IGNORE_NICKS   = 0x00002000,
    IGNORE_DCC     = 0x00004000,
    IGNORE_DCCMSGS = 0x00008000,
    IGNORE_HILIGHT = 0x00010000,
    IGNORE_ALL     = 0x0001FFFF,
};

extern mowgli_dictionary_t *ignores;

typedef struct {
    gchar *mask;
    gint32 levels;
    GPatternSpec *spec;
} ignore_entry;

ignore_entry *ignore_exists(gchar *mask);
gboolean ignore_set(gchar *mask, gint32 levels);
void ignore_showlist(session *sess);
gboolean ignore_del(gchar *mask);
gboolean ignore_check(gchar *mask, gint32 levels);
void ignore_load(void);
void ignore_save(void);
void ignore_gui_open(void);
void ignore_gui_update(gint32 levels);
gint flood_check(gchar *nick, gchar *ip, server *serv, session *sess, gint what);

#endif

