include ../mk/rules.mk
include ../mk/init.mk

localedir = $(datadir)/locale
gettextsrcdir = $(datadir)/gettext/intl
aliaspath = $(localedir)

OBJECTIVE_LIBS_NOINST = libintl.a

DEFS += -DLOCALEDIR=\"$(localedir)\" -DLOCALE_ALIAS_PATH=\"$(aliaspath)\" \
	-DLIBDIR=\"$(libdir)\" -DIN_LIBINTL \
	-DENABLE_RELOCATABLE=1 -DIN_LIBRARY -DINSTALLDIR=\"$(libdir)\" -DINSTALLPREFIX=\"$(libdir)\" -DNO_XMALLOC \
	-Dset_relocation_prefix=libintl_set_relocation_prefix \
	-Drelocate=libintl_relocate \
	-DDEPENDS_ON_LIBICONV=1

CFLAGS += $(PICFLAGS) -I../ -I../.. -I. $(DEFS)

SOURCES = \
	bindtextdom.c \
	dcgettext.c \
	dcigettext.c \
	dcngettext.c \
	dgettext.c \
	dngettext.c \
	explodename.c \
	finddomain.c \
	gettext.c \
	hash-string.c \
	intl-compat.c \
	intl-exports.c \
	l10nflist.c \
	langprefs.c \
	loadmsgcat.c \
	localcharset.c \
	localealias.c \
	localename.c \
	lock.c \
	log.c \
	ngettext.c \
	os2compat.c \
	osdep.c \
	plural.c \
	plural-exp.c \
	printf-args.c \
	printf.c \
	printf-parse.c \
	relocatable.c \
	textdomain.c \
	vasnprintf.c \
	version.c

OBJECTS = ${SOURCES:.c=.o}

include ../mk/objective.mk
