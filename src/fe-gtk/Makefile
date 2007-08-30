include ../../mk/rules.mk
include ../../mk/init.mk

localedir = $(datadir)/locale

OBJECTIVE_BINS = conspire-gtk

CFLAGS += -I../.. $(GUI_CFLAGS) -DG_DISABLE_CAST_CHECKS -DLOCALEDIR=\"$(localedir)\"

LIBADD = -L../common -lconspire $(COMMON_LIBS) $(GUI_LIBS)

SOURCES = about.c ascii.c banlist.c chanlist.c chanview.c custom-list.c \
	dccgui.c editlist.c fe-gtk.c fkeys.c gtkutil.c ignoregui.c menu.c \
	maingui.c notifygui.c palette.c pixmaps.c plugin-tray.c $(plugingui_c) \
	rawlog.c search.c servlistgui.c setup.c sexy-spell-entry.c \
	sync-menu.c textgui.c \
	urlgrab.c userlistgui.c xtext.c

OBJECTS = ${SOURCES:.c=.o}

include ../../mk/objective.mk

