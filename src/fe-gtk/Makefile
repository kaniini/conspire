include ../../extra.mk

localedir = $(datadir)/locale

PROG = conspire
SRCS = about.c ascii.c banlist.c chanlist.c chanview.c custom-list.c \
	dccgui.c editlist.c fe-gtk.c fkeys.c gossip-cell-renderer-expander.c gtkutil.c ignoregui.c menu.c \
	maingui.c notifygui.c palette.c pixmaps.c tray.c $(plugingui_c) \
	rawlog.c search.c servlistgui.c setup.c \
	sync-menu.c textgui.c \
	urlgrab.c userlistgui.c xtext.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../../.. $(COMMON_CFLAGS) $(GUI_CFLAGS) -DG_DISABLE_CAST_CHECKS -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\"
LIBS += -L../common -lconspire -L../libcontrast -lcontrast $(COMMON_LIBS) $(GUI_LIBS)

