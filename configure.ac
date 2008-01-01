dnl Process this file with autoconf to produce a configure script.

AC_INIT([conspire],[0.10.0],[http://conspire.atheme.org/],[conspire])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_PREREQ([2.59])
AC_COPYRIGHT([Copyright (C) 1998-2007 atheme.org])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 subdir-objects no-define])

AC_GNU_SOURCE

AM_MAINTAINER_MODE
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_LN_S
AM_PROG_AS
AM_PROG_MKDIR_P

AC_PATH_PROG(RM, rm)
AC_PATH_PROG(MV, mv)
AC_PATH_PROG(CP, cp)
AC_PATH_PROG(AR, ar)
AC_PATH_PROG(RANLIB, ranlib)

dnl -----------------------------------------------------------
dnl Language Support
dnl -----------------------------------------------------------
GETTEXT_PACKAGE=conspire
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[Gettext package name])

ALL_LINGUAS="be ca cs de el es fi fr gl hu it ja ko lt mk nl pa pt ru sq sr sv uk vi zh_CN zh_TW"
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.16.1])

dnl displaced from acconfig.h
AH_VERBATIM([OLD_PERL],[#undef OLD_PERL])
AH_VERBATIM([PREFIX],[#undef PREFIX])
AH_VERBATIM([XCHATLIBDIR],[#undef XCHATLIBDIR])
AH_VERBATIM([XCHATSHAREDIR],[#undef XCHATSHAREDIR])
AH_VERBATIM([SOCKS],[#undef SOCKS])
AH_VERBATIM([USE_SHM],[#undef USE_SHM])
AH_VERBATIM([USE_GTKSPELL],[#undef USE_GTKSPELL])
AH_VERBATIM([USE_LIBSEXY],[#undef USE_LIBSEXY])
AH_VERBATIM([USE_IPV6],[#undef USE_IPV6])
AH_VERBATIM([USE_MMX],[#undef USE_MMX])
AH_VERBATIM([USE_PLUGIN],[#undef USE_PLUGIN])
AH_VERBATIM([USE_SIGACTION],[#undef USE_SIGACTION])
AH_VERBATIM([USING_FREEBSD],[#undef USING_FREEBSD])
AH_VERBATIM([USING_LINUX],[#undef USING_LINUX])
AH_VERBATIM([USE_DBUS],[#undef USE_DBUS])

AC_PATH_PROG(sedpath, sed)
if test "_$sedpath" = _; then
	AC_MSG_ERROR("Cannot find sed: I need it\!")
fi

AC_PATH_PROG(unamepath, uname)
if test "_$unamepath" = _; then
	system="unknown"
else
	AC_MSG_CHECKING(system type)
	system=`$unamepath -s`
	AC_MSG_RESULT($system)
	if test "$system" = "Linux"; then
		AC_DEFINE(USING_LINUX)
	fi
	if test "$system" = "FreeBSD"; then
		AC_DEFINE(USING_FREEBSD)
	fi
fi

dnl plugin stuff
dnl Ok, first we find out what flags we need to use.
AC_MSG_CHECKING([for what extension and flags to use for plugin compilation])
case "$target" in
        *-apple-*)
                AC_MSG_RESULT([Mac OS X: -fPIC -bundle -fno-common -flat_namespace -undefined suppress, .dylib])
                PICFLAGS="-fPIC -DPIC"
                PICLDFLAGS="-fPIC -DPIC -bundle -fno-common -flat_namespace -undefined suppress"
                LIBLDFLAGS="-dynamiclib -flat_namespace -undefined suppress -single_module"
                AUDLDFLAGS=""
		GUI_LIBS="$GUI_LIBS -framework Carbon"
                SHARED_SUFFIX=".dylib"
                ;;
        *)
                AC_MSG_RESULT([libdl-compatible: -fPIC -DPIC -shared, .so])
                PICFLAGS="-fPIC -DPIC"
                PICLDFLAGS="-fPIC -DPIC -shared"
                LIBLDFLAGS="-fPIC -DPIC -shared"
                AUDLDFLAGS="-Wl,-export-dynamic"
                SHARED_SUFFIX=".so"
                ;;
esac
AC_SUBST(PICFLAGS)
AC_SUBST(PICLDFLAGS)
AC_SUBST(LIBLDFLAGS)
AC_SUBST(AUDLDFLAGS)
AC_SUBST(SHARED_SUFFIX)
AC_DEFINE_UNQUOTED(SHARED_SUFFIX, "${SHARED_SUFFIX}",
        [Define the shared module suffix extension on your platform.])


dnl *********************************************************************
dnl ** configure switches ***********************************************
dnl *********************************************************************

AC_ARG_ENABLE(socks,
[  --enable-socks          link with SOCKS5 library (default: no)],
        socks=$enableval, socks=no)

AC_ARG_ENABLE(ipv6,
[  --disable-ipv6            disable IPv6 (default: enabled)],
        ipv6=$enableval, ipv6=yes)

AC_ARG_ENABLE(gnutls,
[  --enable-gnutls           enable use of GnuTLS],
        gnutls=$enableval, [gnutls=yes])

AC_ARG_ENABLE(gtkfe,
[  --disable-gtkfe         disable building gtk frontend],
        gtkfe=$enableval, gtkfe=yes)

AC_ARG_ENABLE(python,
[  --disable-python        don't build the python plugin],
        python=$enableval, python=yes)

AC_ARG_ENABLE(plugin,
[  --disable-plugin        disable plugin support],
        plugin=$enableval, plugin=yes)

AC_ARG_ENABLE(mmx,
[  --disable-mmx           disable MMX assembly routines],
        mmx=$enableval, mmx=yes)

AC_ARG_ENABLE(spell,
[  --enable-spell=type     enable spelling type: none static libsexy gtkspell],
		  spell=$enableval, spell=libsexy)

dnl *********************************************************************
dnl ** GLIB *************************************************************
dnl *********************************************************************

AM_PATH_GLIB_2_0(2.0.3, glib=yes, glib=no)
if test "$glib" = no; then
	AC_MSG_ERROR("Cannot find glib")
fi

COMMON_CFLAGS="$GLIB_CFLAGS"
COMMON_LIBS="$GLIB_LIBS"

dnl *********************************************************************
dnl ** GTK **************************************************************
dnl *********************************************************************

AM_PATH_GTK_2_0(2.10.0, havegtk=yes, havegtk=no)

if test "$havegtk" = no; then
	gtkfe=no
	echo
	echo Cannot find GTK\! Not building GTK FrontEnd.
	echo
fi

if test "$gtkfe" != yes; then
	COMMON_LIBS="$GLIB_LIBS"
	COMMON_CFLAGS="$GLIB_CFLAGS"
fi

dnl *********************************************************************
dnl ** Mowgli ***********************************************************
dnl *********************************************************************

PKG_CHECK_MODULES(MOWGLI, [libmowgli >= 0.6.0], mowgli=yes, mowgli=no)
if test "$mowgli" = no; then
	AC_MSG_ERROR("Cannot find acceptable version of mowgli. http://atheme.org/projects/mowgli.shml")
fi

COMMON_CFLAGS="$GLIB_CFLAGS $MOWGLI_CFLAGS"
COMMON_LIBS="$GLIB_LIBS $MOWGLI_LIBS"

dnl *********************************************************************
dnl ** IPv6 *************************************************************
dnl *********************************************************************

dnl purely for Solaris
AC_CHECK_FUNC(select, ,
	AC_CHECK_LIB(socket, select, ,
		AC_CHECK_LIB(nsl, select, ,
			AC_CHECK_LIB(inet, select, ,
				AC_CHECK_LIB(cposix, select, ,
					AC_CHECK_LIB(net, select, ,
						AC_MSG_WARN(i can not find select.  you might need to help me)))))))
AC_CHECK_LIB(socket, select)

if test "$ipv6" = yes; then
	AC_CHECK_FUNCS(getaddrinfo, have_getaddrinfo=yes)
	AC_MSG_CHECKING(whether to enable IPv6 support)
	if test "$have_getaddrinfo" = yes; then
		AC_MSG_RESULT(yes)
		AC_DEFINE(USE_IPV6)
	else
		ipv6=no
		AC_MSG_RESULT(no)
	fi
fi

BUILDSYS_PROG_IMPLIB
BUILDSYS_SHARED_LIB

dnl ** GnuTLS

if test "$gnutls" = "yes"; then
	PKG_CHECK_MODULES(GNUTLS, [gnutls], [
		COMMON_LIBS="$COMMON_LIBS $GNUTLS_LIBS"
		CPPFLAGS="$CPPFLAGS -DGNUTLS $GNUTLS_CFLAGS"
	], [gnutls=no])
fi

dnl *********************************************************************
dnl ** PLUGIN ***********************************************************
dnl *********************************************************************

if test "$plugin" = yes; then
	AC_DEFINE(USE_PLUGIN)
	plugingui_c="plugingui.c"
	AC_SUBST(plugingui_c)
	AC_PATH_PROG(pkgconfigpath, pkg-config)
	dnl we just need the -Wl,--export-dynamic, but not -lgmodule-2.0
	RDYNAMIC_FLAGS="`$pkgconfigpath gmodule-2.0 --libs`"
	LIBS="$LIBS $RDYNAMIC_FLAGS"
fi

dnl #######################################################################
dnl # Check for DBUS libraries
dnl #######################################################################

PKG_CHECK_MODULES(DBUS, [dbus-1 >= 0.60 dbus-glib-1 >= 0.60 gthread-2.0], dbus=yes, [
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([DBus is required by Conspire.])
	dbus=no
])
AC_PATH_PROG(DBUS_BINDING_TOOL, dbus-binding-tool, no)
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal, no)
if test "x$DBUS_BINDING_TOOL" = "xno" || test "x$GLIB_GENMARSHAL" = "xno" || test "x$dbus" = "xno" ; then
	dbus="no"
else
	COMMON_LIBS="$COMMON_LIBS $DBUS_LIBS"
	COMMON_CFLAGS="$COMMON_CFLAGS $DBUS_CFLAGS"
	AC_DEFINE(USE_DBUS)
	AS_AC_EXPAND(DATADIR, $datadir)

	XCHATWRAP="libconspire-xchatemu xchatwrap"
	DBUSDIR="dbus"
	DBUSLIBADD="-Ldbus/ -lconspire-dbus"
	AC_SUBST(DBUSDIR)
	AC_SUBST(DBUSLIBADD)
	AC_SUBST(XCHATWRAP)

	DBUS_SERVICES_DIR="$DATADIR/dbus-1/services"
	AC_SUBST(DBUS_SERVICES_DIR)
	AC_DEFINE_UNQUOTED(DBUS_SERVICES_DIR, "$DBUS_SERVICES_DIR", [Where services dir for DBUS is])
fi

dnl ** Python **

if test "$dbus" = no; then
	python=no
fi

if test "$python" = yes; then
        AC_PATH_PROG(pythonpath, python2)
        if test "_$pythonpath" = _ ; then
                AC_PATH_PROG(pythonpath, python)
        fi
        if test "_$pythonpath" = _ ; then
                python=no
        else
                AC_MSG_CHECKING(Python version)
                changequote(<<, >>)dnl
                PY_VER=`$pythonpath -c 'import distutils.sysconfig; print distutils.sysconfig.get_config_vars("VERSION")[0];'`
                PY_LIB=`$pythonpath -c 'import distutils.sysconfig; print distutils.sysconfig.get_python_lib(standard_lib=1);'`
                PY_INC=`$pythonpath -c 'import distutils.sysconfig; print distutils.sysconfig.get_config_vars("INCLUDEPY")[0];'`
                $pythonpath -c "import sys; map(int,sys.version[:3].split('.')) >= [2,2] or sys.exit(1)"
                changequote([, ])dnl
                AC_MSG_RESULT($PY_VER)
                if test "$?" != "1"; then
                        AC_MSG_CHECKING(Python compile flags)
                        PY_PREFIX=`$pythonpath -c 'import sys; print sys.prefix'`
                        PY_EXEC_PREFIX=`$pythonpath -c 'import sys; print sys.exec_prefix'`
                        if test -f $PY_INC/Python.h; then
                                PY_LIBS="-L$PY_LIB/config -lpython$PY_VER -lpthread -lutil"
                                PY_CFLAGS="-I$PY_INC"
				XCHATWRAP="$XCHATWRAP conspire-python"
                                AC_MSG_RESULT(ok)
                        else
                                python=no
                                AC_MSG_RESULT([Can't find Python.h])
                        fi
                else
                        echo "Python too old. Only 2.2 or above is supported."
                        python=no
                fi
        fi
fi

dnl *********************************************************************
dnl ** CONDITIONALS *****************************************************
dnl *********************************************************************

AM_CONDITIONAL(USE_LIBSEXY, test "x$spell" = "xstatic")
AM_CONDITIONAL(DO_GTK, test "x$gtkfe" = "xyes")
AM_CONDITIONAL(DO_PLUGIN, test "x$plugin" = "xyes")
AM_CONDITIONAL(USE_DBUS, test "x$dbus" = "xyes")
AM_CONDITIONAL(DO_GCONF, test "x$GCONFTOOL" != "xno")

dnl *********************************************************************
dnl ** SOCKS5 ***********************************************************
dnl *********************************************************************

if test "$socks" = yes; then
	socks=no
	AC_CHECK_LIB(socks5, SOCKSconnect, have_socks=yes)
	if test "$have_socks" = yes; then
		AC_CHECK_HEADERS(socks.h, have_socks_h=yes)
		if test "$have_socks_h" = yes; then
			socks=yes
			AC_DEFINE(SOCKS)
			LIBS="$LIBS -lsocks5"
		fi
	fi
fi

dnl *********************************************************************
dnl ** MMX **************************************************************
dnl *********************************************************************

dnl we don't need mmx on *this* machine, just i386, because
dnl it's checked at runtime.
if test "$mmx" = "yes"; then
	case $host_cpu in
    i386|i486|i586|i686|i786|k6|k7)
    mmx=yes
        ;;
     *)
    mmx=no
	esac
	if test "$system" = "OpenBSD"; then
		dnl openbsd fails because mmx_cmod doesn't prefix its symbols with underscore.
		dnl xtext.o: Undefined symbol `_shade_ximage_15_mmx' referenced from text segment
		mmx=no
	fi
	if test "$mmx" = "yes"; then
		AC_DEFINE(USE_MMX)
	fi
fi

AM_CONDITIONAL(USE_MMX, test "$mmx" = "yes")

dnl *********************************************************************
dnl ** GCC FLAGS ********************************************************
dnl *********************************************************************

dnl Only use -Wall and -pipe if we have gcc
if test "x$GCC" = "xyes"; then
	if test -z "`echo "$CFLAGS" | grep "\-Wall" 2> /dev/null`" ; then
		CFLAGS="$CFLAGS -Wall"
	fi
	if test "$system" = "Linux" -o "$system" = "FreeBSD"; then
		if test -z "`echo "$CFLAGS" | grep "\-pipe" 2> /dev/null`" ; then
			CFLAGS="$CFLAGS -pipe"
		fi
	fi
	if test -z "`echo "$CFLAGS" | grep "\-g " 2> /dev/null`" ; then
		CFLAGS="$CFLAGS -g"
	fi
fi

dnl does this compiler support -Wno-pointer-sign ?
AC_MSG_CHECKING([if gcc accepts -Wno-pointer-sign ])

safe_CFLAGS=$CFLAGS
CFLAGS="-Wno-pointer-sign"

AC_TRY_COMPILE(, [
int main () { return 0 ; }
],
[
no_pointer_sign=yes
AC_MSG_RESULT([yes])
], [
no_pointer_sign=no
AC_MSG_RESULT([no])
])
CFLAGS=$safe_CFLAGS

if test x$no_pointer_sign = xyes; then
	CFLAGS="$CFLAGS -Wno-pointer-sign"
fi

dnl does this compiler support -funsigned-char ?
AC_MSG_CHECKING([if gcc accepts -funsigned-char ])

safe_CFLAGS=$CFLAGS
CFLAGS="-funsigned-char"

AC_TRY_COMPILE(, [
int main () { return 0 ; }
],
[
unsigned_char=yes
AC_MSG_RESULT([yes])
], [
unsigned_char=no
AC_MSG_RESULT([no])
])
CFLAGS=$safe_CFLAGS

if test x$unsigned_char = xyes; then
	CFLAGS="$CFLAGS -funsigned-char"
fi

dnl *********************************************************************
dnl ** FUNCTIONS/LIBS/CFLAGS ********************************************
dnl *********************************************************************

AC_MSG_CHECKING(for modern sigaction)
dnl libc5 on linux and FreeBSD 3.x doesn't have siginfo_t
dnl and the sa_sigation field.
AC_TRY_COMPILE(
	[#include <signal.h>],
	[struct sigaction act;
	 siginfo_t *si;
	 act.sa_sigaction = 0;],
	[
		AC_MSG_RESULT(yes)
		AC_DEFINE(USE_SIGACTION) 
	],
	AC_MSG_RESULT(no))

AC_PATH_PROG(gdkpixbufcsourcepath, gdk-pixbuf-csource)
AC_SUBST(gdkpixbufcsourcepath)
if test "$gtkfe" != no -a "_$gdkpixbufcsourcepath" = _; then
	AC_MSG_ERROR("Cannot find gdk-pixbuf-csource: Install GTK+ 2.0\!")
fi

dnl if we don't have this, use g_snprintf instead
AC_CHECK_FUNCS(snprintf vsnprintf memrchr strtoull)

AC_CHECK_FUNC(gethostbyname, ,
	AC_CHECK_LIB(resolv, gethostbyname, ,
		AC_CHECK_LIB(nsl, gethostbyname)))

AC_CHECK_FUNC(gethostname, , AC_CHECK_LIB(nsl, gethostname))

dnl necessary for IRIX
AC_CHECK_HEADERS(strings.h)

dnl Mac OS X and Darwin use lookupd, which caches DNS queries by default
AC_EGREP_CPP(lookupd, dnl
[#if (defined(__APPLE__) && defined(__MACH__))
	lookupd
#endif], AC_DEFINE([LOOKUPD],1,[Define to 1 if the system uses lookupd]))

dnl freebsd needs this
LIBS="$LIBS $INTLLIBS"
CFLAGS="$CFLAGS $CPPFLAGS"

GUI_CFLAGS="$GTK_CFLAGS $COMMON_CFLAGS"
GUI_LIBS="$GTK_LIBS $COMMON_LIBS -lm"

dnl make these visible to all Makefiles
AC_SUBST(GUI_LIBS)
AC_SUBST(GUI_CFLAGS)
AC_SUBST(COMMON_LIBS)
AC_SUBST(COMMON_CFLAGS)
AC_SUBST(PERL_CFLAGS)
AC_SUBST(PERL_LDFLAGS)
AC_SUBST(PY_CFLAGS)
AC_SUBST(PY_LIBS)
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)


PLUGIN_INCLUDES='-I$(top_srcdir)/plugins'
AC_SUBST(PLUGIN_INCLUDES)

dnl for plugin.c and pixmaps.c
test "x$prefix" = xNONE && prefix="$ac_default_prefix"
test "x$exec_prefix" = xNONE && exec_prefix="$prefix"

AC_DEFINE_UNQUOTED(PREFIX, "${prefix}")

XCHATLIBDIR=`eval echo ${libdir}/xchat`
AC_DEFINE_UNQUOTED(XCHATLIBDIR, "$XCHATLIBDIR")

XCHATSHAREDIR=`eval echo ${datadir}`
AC_DEFINE_UNQUOTED(XCHATSHAREDIR, "$XCHATSHAREDIR")

dnl for plugins/xxx/Makefile.am
dnl USING RPATHS IS WRONG -nenolod
xchatlibdir=${libdir}/conspire
AC_SUBST(xchatlibdir)

AC_OUTPUT([
po/Makefile.in
buildsys.mk
extra.mk
conspire
])

echo
echo "Configuration:"
echo
echo "  Install path:                           ${prefix}"
echo
echo "  GnuTLS support:                         ${gnutls}"
echo "  DBus support:                           ${dbus}"
echo "  IPv6 support:                           ${ipv6}"
echo "  Plugin loader:                          ${plugin}"
echo
