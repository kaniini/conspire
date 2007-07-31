include ../../../mk/rules.mk
include ../../../mk/init.mk

OBJECTIVE_LIBS_NOINST = libconspire-adns.a
SOURCES = check.c  event.c  general.c  parse.c  poll.c  query.c  reply.c  setup.c  \
	transmit.c  types.c

CFLAGS += $(PICFLAGS) $(COMMON_CFLAGS)

OBJECTS = ${SOURCES:.c=.o}

include ../../../mk/objective.mk

