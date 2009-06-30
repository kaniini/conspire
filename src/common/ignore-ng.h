#ifndef CONSPIRE_IGNORE_H
#define CONSPIRE_IGNORE_H

typedef enum {
    IGNORE_NONE    = (0),
    IGNORE_EXCEPT  = (1),
    IGNORE_PRIVATE = (1 <<  1),
    IGNORE_PUBLIC  = (1 <<  2),
    IGNORE_NOTICE  = (1 <<  3),
    IGNORE_SNOTES  = (1 <<  4),
    IGNORE_CTCP    = (1 <<  5),
    IGNORE_ACTION  = (1 <<  6),
    IGNORE_JOINS   = (1 <<  7),
    IGNORE_PARTS   = (1 <<  8),
    IGNORE_QUITS   = (1 <<  9),
    IGNORE_KICKS   = (1 << 10),
    IGNORE_MODES   = (1 << 11),
    IGNORE_TOPICS  = (1 << 12),
    IGNORE_INVITES = (1 << 13),
    IGNORE_NICKS   = (1 << 14),
    IGNORE_DCC     = (1 << 15),
    IGNORE_DCCMSGS = (1 << 16),
    IGNORE_HILIGHT = (1 << 17),
    IGNORE_ALL     = 0xFFFFFFFF,
} IgnoreLevel;

typedef struct {
    gchar *mask;
    IgnoreLevel levels;
    GPatternSpec *spec;
} IgnoreEntry;

IgnoreEntry *ignore_find_entry(const gchar *mask);
gboolean ignore_set(const gchar *mask, const IgnoreLevel levels);
void ignore_showlist(session *sess);
gboolean ignore_del(const gchar *mask);
gboolean ignore_check(const gchar *mask, const IgnoreLevel levels);
void ignore_load(void);
void ignore_save(void);
void ignore_gui_open(void);
void ignore_gui_update(const IgnoreLevel levels);
gint flood_check(gchar *nick, gchar *ip, server *serv, session *sess, gint what);

#endif

