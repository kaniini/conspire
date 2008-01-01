SRCS = python.c

PROG = conspire-python

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../.. $(COMMON_CFLAGS) $(PY_CFLAGS)

LIBS += $(AUDLDFLAGS) $(PY_LIBS) $(DBUS_LIBS) -lgmodule-2.0 -ldl -lglib-2.0 -lm -L../libconspire-xchatemu -lconspire-xchatemu

bindir = ${plugindir}
