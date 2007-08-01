include ../../mk/rules.mk
include ../../mk/init.mk

localedir = $(datadir)/locale

OBJECTIVE_BINS = conspire-xchatwrap

CFLAGS += -I../.. $(COMMON_CFLAGS)

# THIS IS WRONG!
LIBADD = $(DBUS_LIBS) -Wl,--export-dynamic -lgmodule-2.0 -ldl -lglib-2.0 -lm

SOURCES = conspire-xchatwrap.c

OBJECTS = ${SOURCES:.c=.o}

include ../../mk/objective.mk

