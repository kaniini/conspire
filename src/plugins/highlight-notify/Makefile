include ../../../extra.mk

PLUGIN = highlight-notify${PLUGIN_SUFFIX}
SRCS = highlight-notify.c

include ../../../buildsys.mk

plugindir := ${plugindir}/plugins

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(GUI_CFLAGS) -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\"
LIBS += $(COMMON_LIBS) $(GUI_LIBS)

