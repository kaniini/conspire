/* cfgfiles.h */

#ifndef XCHAT_CFGFILES_H
#define XCHAT_CFGFILES_H

#include "xchat.h"
#include <stdio.h>

extern char *xdir_fs;
extern char *xdir_utf;

char *cfg_get_str (char *cfg, const char *var, char **dest);
int cfg_get_bool (char *var);
int cfg_get_int_with_result (char *cfg, const char *var, int *result);
int cfg_get_int (char *cfg, const char *var);
int cfg_put_int (int fh, int value, const char *var);
int cfg_get_color (char *cfg, char *var, int *r, int *g, int *b);
int cfg_put_color (int fh, int r, int g, int b, char *var);
char *get_xdir_fs (void);
char *get_xdir_utf8 (void);
void load_config (void);
int save_config (void);
void list_free (GSList ** list);
void list_loadconf (char *file, GSList ** list, char *defaultconf);
int list_delentry (GSList ** list, char *name);
void list_addentry (GSList ** list, char *cmd, char *name);
int cmd_set (session *sess, char *tbuf, char *word[], char *word_eol[]);
int xchat_open_file (char *file, int flags, int mode, int xof_flags);
FILE *xchat_fopen_file (const char *file, const char *mode, int xof_flags);
#define XOF_DOMODE 1
#define XOF_FULLPATH 2

typedef struct _PrefsEntry PrefsEntry;

typedef enum {
	PREFS_TYPE_STR,
	PREFS_TYPE_INT,
	PREFS_TYPE_BOOL
} PrefsType;

/* to set int/boolean use G_INT_TO_POINTER() */
typedef void (*PrefsSetter)(PrefsEntry *, void *);

struct _PrefsEntry {
	const char *name;
	PrefsType type;
	void *ptr;
	PrefsSetter set;
};

#endif
