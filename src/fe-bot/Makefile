include ../../mk/rules.mk
include ../../mk/init.mk

localedir = $(datadir)/locale

OBJECTIVE_BINS = conspire-gtk

CFLAGS += -I../..

LIBADD = -L../common -lconspire $(COMMON_LIBS)

SOURCES = conspire-bot.c

OBJECTS = ${SOURCES:.c=.o}

include ../../mk/objective.mk

