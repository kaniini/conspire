include ../../mk/init.mk
include ../../mk/rules.mk

OBJECTIVE_LIBS = libconspire.so
OBJECTIVE_SONAME_SUFFIX = 0
LIBCONSPIRE_SUFFIX = 0.1.0

CFLAGS += $(PICFLAGS) $(COMMON_CFLAGS)

SUBDIRS = tools $(DBUSDIR)
LIBADD += $(DBUSLIBADD)

SOURCES = cfgfiles.c ctcp.c dcc.c history.c ignore.c \
	inbound.c modes.c network.c notify.c outbound.c \
	plugin.c plugin-timer.c proto-irc.c server.c servlist.c $(ssl_c) \
	text.c tree.c url.c userlist.c util.c xchat.c

OBJECTS = ${SOURCES:.c=.o}

include ../../mk/objective.mk

install-posthook:
	@mv ${DESTDIR}/${LIBDIR}/libconspire$(SHARED_SUFFIX) ${DESTDIR}/${LIBDIR}/libconspire$(SHARED_SUFFIX).$(LIBCONSPIRE_SUFFIX)
	@ln -sf libconspire$(SHARED_SUFFIX).$(LIBCONSPIRE_SUFFIX) \
		${DESTDIR}/${LIBDIR}/libconspire$(SHARED_SUFFIX).1
	@ln -sf libconspire$(SHARED_SUFFIX).1 \
		${DESTDIR}/${LIBDIR}/libconspire$(SHARED_SUFFIX)

