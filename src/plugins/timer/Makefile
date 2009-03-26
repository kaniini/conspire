include ../../../extra.mk

PLUGIN = timer${PLUGIN_SUFFIX}
plugindir = ${LIBDIR}/conspire/plugins
SRCS = timer.c

include ../../../buildsys.mk
include ../../../extra.mk

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(GUI_CFLAGS) -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\"
LIBS += $(COMMON_LIBS) $(GUI_LIBS)

