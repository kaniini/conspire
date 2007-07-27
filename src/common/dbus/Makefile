include ../../../mk/rules.mk
include ../../../mk/init.mk

OBJECTIVE_LIBS_NOINST = libconspire-dbus.a
SOURCES = dbus-plugin.c	dbus-client.c

CFLAGS += $(PICFLAGS) $(COMMON_CFLAGS) $(DBUS_CFLAGS)

OBJECTS = ${SOURCES:.c=.o}

include ../../../mk/objective.mk

