include ../../../extra.mk

PLUGIN = autoaway${PLUGIN_SUFFIX}
SRCS = autoaway.c gscreensaver.c xscreensaver.c

include ../../../buildsys.mk
include ../../../extra.mk

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(GUI_CFLAGS) -DG_DISABLE_CAST_CHECKS -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\"
LIBS += $(COMMON_LIBS) $(GUI_LIBS)

