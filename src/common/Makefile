include ../../extra.mk

SUBDIRS = tools

LIB = ${LIB_PREFIX}conspire${LIB_SUFFIX}
LIB_MAJOR=0
LIB_MINOR=1

SRCS = base64.c cfgfiles.c ctcp.c dcc.c history.c ignore.c \
	inbound.c modes.c network.c notify.c outbound.c \
	plugin.c plugin-timer.c proto-irc.c server.c servlist.c \
	text.c tree.c url.c userlist.c util.c xchat.c dbus/dbus-plugin.c dbus/dbus-client.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += $(COMMON_CFLAGS)
CFLAGS += $(PLUGIN_CFLAGS)
LIBS += $(COMMON_LIBS)
