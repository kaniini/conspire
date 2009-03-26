#include "format.h"

#ifndef XCHAT_TEXT_H
#define XCHAT_TEXT_H

struct text_event
{
	char *name;
	int num_args;
	char *def;
};

void scrollback_close (session *sess);
void scrollback_load (session *sess);

int text_word_check (char *word, int len);
void PrintText (session *sess, char *text);
void PrintTextf (session *sess, char *format, ...);
void log_close (session *sess);
void log_open (session *sess);
void load_text_events (void);
void pevent_save (char *fn);
int pevt_build_string (const char *input, char **output, int *max_arg);
int pevent_load (char *filename);
void pevent_make_pntevts (void);
void text_emit (int index, session *sess, char *a, char *b, char *c, char *d);
int text_emit_by_name (char *name, session *sess, char *a, char *b, char *c, char *d);
char *text_validate (char **text, int *len);
int get_stamp_str (char *fmt, time_t tim, char **ret);
void format_event (session *sess, int index, char **args, char *o, int sizeofo, unsigned int stripcolor_args);
char *text_find_format_string (char *name);
 
void sound_beep (session *);

#endif
