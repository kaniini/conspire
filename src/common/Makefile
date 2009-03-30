include ../../extra.mk

SUBDIRS = tools

LIB = ${LIB_PREFIX}conspire${LIB_SUFFIX}
LIB_MAJOR=0
LIB_MINOR=1

SRCS = \
base64.c cap.c cfgfiles.c ctcp.c command_factory.c dcc.c format.c history.c \
ignore.c inbound.c linequeue.c modes.c network.c notify.c outbound.c \
plugin.c proto-irc.c sasl.c server.c servlist.c session.c signal_factory.c \
signal_printer.c text.c tree.c upnp.c url.c userlist.c util.c xchat.c \
upnp/igd_desc_parse.c upnp/minisoap.c upnp/minissdpc.c upnp/miniupnpc.c \
upnp/miniwget.c upnp/minixml.c upnp/upnpcommands.c upnp/upnpreplyparse.c

INCLUDES = \
base64.h cap.h cfgfiles.h command_factory.h conspire-config.h ctcp.h dcc.h \
debug.h fe.h format.h history.h ignore.h inbound.h inet.h linequeue.h modes.h \
network.h notify.h outbound.h plugin.h proto-irc.h sasl.h server.h servlist.h \
session.h signal_factory.h signal_printer.h text.h textevents.h tree.h upnp.h \
url.h userlist.h util.h xchat.h xchatc.h

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += $(COMMON_CFLAGS)
CFLAGS += $(PLUGIN_CFLAGS)
LIBS += $(COMMON_LIBS)
