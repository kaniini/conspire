SRCS = conspire-xchatwrap.c

PROG = conspire-xchatwrap

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../.. $(COMMON_CFLAGS)

LIBS += $(AUDLDFLAGS) $(DBUS_LIBS) -lgmodule-2.0 -ldl -lglib-2.0 -lm

