include ../../extra.mk

SUBDIRS = tools

LIB = ${LIB_PREFIX}conspire${LIB_SUFFIX}
LIB_MAJOR=0
LIB_MINOR=1

SRCS = base64.c cfgfiles.c ctcp.c command_factory.c dcc.c history.c ignore.c \
	inbound.c modes.c network.c notify.c outbound.c \
	plugin.c proto-irc.c server.c servlist.c signal_factory.c signal_printer.c \
	text.c timer.c tree.c upnp.c url.c userlist.c util.c xchat.c dbus/dbus-plugin.c dbus/dbus-client.c \
        upnp/igd_desc_parse.c upnp/minisoap.c upnp/minissdpc.c upnp/miniupnpc.c upnp/miniwget.c upnp/minixml.c upnp/upnpcommands.c upnp/upnpreplyparse.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += $(COMMON_CFLAGS)
CFLAGS += $(PLUGIN_CFLAGS)
LIBS += $(COMMON_LIBS)
