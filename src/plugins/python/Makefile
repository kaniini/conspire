include ../../../extra.mk

PLUGIN = python${PLUGIN_SUFFIX}
SRCS = python.c

include ../../../buildsys.mk
include ../../../extra.mk

plugindir := ${plugindir}/plugins

CFLAGS += ${PLUGIN_CFLAGS} 
CPPFLAGS += -I../../.. -I../.. -I../../common $(COMMON_CFLAGS) $(PY_CFLAGS) $(GUI_CFLAGS) -DG_DISABLE_CAST_CHECKS -DLOCALEDIR=\"$(localedir)\" -DSHAREDIR=\"$(prefix)/share\"
LIBS += $(COMMON_LIBS) $(PY_LIBS) $(GUI_LIBS) 

