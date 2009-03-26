include ../../../extra.mk

PLUGIN = javascript${PLUGIN_SUFFIX}
plugindir = ${LIBDIR}/conspire/plugins
SRCS = javascript.c

include ../../../buildsys.mk
include ../../../extra.mk

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(GUI_CFLAGS) -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\" $(DBUS_CFLAGS)
LIBS += $(COMMON_LIBS) $(GUI_LIBS) $(DBUS_LIBS) -lmozjs

