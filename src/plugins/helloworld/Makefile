include ../../../extra.mk

PLUGIN = helloworld${PLUGIN_SUFFIX}
plugindir = ${LIBDIR}/conspire/plugins
SRCS = helloworld.c

include ../../../buildsys.mk
include ../../../extra.mk

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(GUI_CFLAGS) -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\"
LIBS += $(COMMON_LIBS) $(GUI_LIBS)

