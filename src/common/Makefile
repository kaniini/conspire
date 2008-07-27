include ../../extra.mk

SUBDIRS = tools

LIB = ${LIB_PREFIX}conspire${LIB_SUFFIX}
LIB_MAJOR=0
LIB_MINOR=1

SRCS = base64.c cap.c cfgfiles.c ctcp.c command_factory.c dcc.c history.c ignore.c \
	inbound.c linequeue.c modes.c network.c notify.c outbound.c \
	plugin.c proto-irc.c sasl.c server.c servlist.c signal_factory.c signal_printer.c \
	text.c timer.c tree.c upnp.c url.c userlist.c util.c xchat.c \
        upnp/igd_desc_parse.c upnp/minisoap.c upnp/minissdpc.c upnp/miniupnpc.c upnp/miniwget.c upnp/minixml.c upnp/upnpcommands.c upnp/upnpreplyparse.c

INCLUDES = \
base64.h  cap.h ctcp.h   history.h  modes.h     plugin.h     servlist.h        textevents.h  upnp.h      xchatc.h \
cfgfiles.h         dcc.h    ignore.h  linequeue.h  network.h   proto-irc.h  signal_factory.h  text.h        url.h       xchat.h \
command_factory.h  debug.h  inbound.h  notify.h    sasl.h       signal_printer.h  timer.h       userlist.h \
fe.h     inet.h     outbound.h  server.h     textenums.h       tree.h        util.h	conspire-config.h

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += $(COMMON_CFLAGS)
CFLAGS += $(PLUGIN_CFLAGS)
LIBS += $(COMMON_LIBS)
