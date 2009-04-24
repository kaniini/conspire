include ../../../extra.mk

PLUGIN = autoaway${PLUGIN_SUFFIX}
SRCS = autoaway.c gscreensaver.c xscreensaver.c

include ../../../buildsys.mk

plugindir := ${plugindir}/plugins

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(GUI_CFLAGS) -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\" $(DBUS_CFLAGS)
LIBS += $(COMMON_LIBS) $(GUI_LIBS) $(DBUS_LIBS)

