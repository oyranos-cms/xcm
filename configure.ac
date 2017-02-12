define([MAJOR],[0])
define([MINOR],[5])
define([MICRO],[5])
AC_INIT([xcm], [MAJOR.MINOR.MICRO],
         https://github.com/oyranos-cms/libxcm/issues,
         xcm,
         http://www.oyranos.org)
LIBTOOL_CURRENT=0
LIBTOOL_REVISION=5
LIBTOOL_AGE=0

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AM_PROG_LIBTOOL
AM_SILENT_RULES

AC_SUBST(LIBTOOL_VERSION)
AC_SUBST(XCM_PACKAGE_MAJOR)
AC_SUBST(XCM_PACKAGE_MINOR)
AC_SUBST(XCM_PACKAGE_MICRO)
AC_SUBST(PACKAGE_VERSION)
AC_SUBST(PACKAGE_RELEASE)
AC_SUBST(HAVE_X11)
AC_SUBST(HAVE_LINUX)
AC_SUBST(HAVE_OY)

XCM_PACKAGE_MAJOR=MAJOR
XCM_PACKAGE_MINOR=MINOR
XCM_PACKAGE_MICRO=MICRO
PACKAGE_RELEASE=0

LIBTOOL_VERSION=$LIBTOOL_CURRENT:$LIBTOOL_REVISION:$LIBTOOL_AGE

AC_MSG_CHECKING([wether to enable x11 support])
AC_ARG_WITH(x11,
    [  --with-x11              enable x11 support],
    with_x11=$withval,
    with_x11=yes)
AC_MSG_RESULT($with_x11)

if test "$with_x11" = "yes"; then
PKG_CHECK_EXISTS([x11], [
	PKG_CHECK_MODULES([libX11], [x11])
	PKG_CHECK_MODULES([libXfixes], [xfixes])
	PKG_CHECK_MODULES([libXmu], [xmu])
	AM_CONDITIONAL([HAVE_X11], [true])
        HAVE_X11="#define XCM_HAVE_X11 1"
        PKG_CONFIG_PRIVATE_X11="xproto x11"
], [
	AC_MSG_CHECKING([for libX11])
	AC_MSG_RESULT([no])
	AM_CONDITIONAL([HAVE_X11], [false])
        HAVE_X11=
])
else
	AC_MSG_CHECKING([for libX11])
	AC_MSG_RESULT([no])
	AM_CONDITIONAL([HAVE_X11], [false])
	HAVE_X11=
fi

AC_MSG_CHECKING([wether to use oyranos])
AC_ARG_WITH(oyranos,
    [  --with-oyranos          enable oyranos],
    with_oyranos=$withval,
    with_oyranos=yes)
AC_MSG_RESULT($with_oyranos)

if test "$with_oyranos" = "yes"; then
PKG_CHECK_EXISTS([oyranos], [
	PKG_CHECK_MODULES([liboyranos], [oyranos])
	AM_CONDITIONAL([HAVE_OY], [true])
        HAVE_OY="#define XCM_HAVE_OY 1"
], [
	AC_MSG_CHECKING([for liboyranos])
	AC_MSG_RESULT([no])
	AM_CONDITIONAL([HAVE_OY], [false])
        HAVE_OY=
])
else
	AC_MSG_CHECKING([for liboyraos])
	AC_MSG_RESULT([no])
	AM_CONDITIONAL([HAVE_OY], [false])
	HAVE_OY=
fi

PKG_CHECK_EXISTS([xcm], [
	PKG_CHECK_MODULES([libXcm], [xcm])
	PKG_CHECK_MODULES([libXcmEDID], [xcm-edid])
	PKG_CHECK_MODULES([libXcmDDC], [xcm-ddc])
	PKG_CHECK_MODULES([libXcmX11], [xcm-x11])
	AM_CONDITIONAL([HAVE_XCM], [true])
        HAVE_XCM="#define XCM_HAVE_XCM 1"
], [
	AC_MSG_CHECKING([for libXcm])
	AC_MSG_RESULT([no])
	AM_CONDITIONAL([HAVE_XCM], [false])
        HAVE_XCM=
])

AC_PATH_PROGS(RPMBUILD, rpm, :)

LINUX="`uname | grep Linux | wc -l`"
if [[ $LINUX = "1" ]]; then
	HAVE_LINUX="#define XCM_HAVE_LINUX 1"
	AM_CONDITIONAL([HAVE_LINUX], [true])
        udevdir="/lib/udev/rules.d/"
        AC_ARG_WITH([udev-dir],
            AS_HELP_STRING([--with-udev-dir=DIR],[Directory where udev expects helper programs to be located]),
            [udevdir="$with_udev_dir"])
        AC_SUBST([udevdir], [$udevdir])
else
	HAVE_LINUX=
	AM_CONDITIONAL([HAVE_LINUX], [false])
fi



AC_CONFIG_FILES([Makefile src/Makefile xcm.spec xcm_version.h])
AC_OUTPUT

if [[ "$HAVE_XCM" = "" ]]; then
echo "HAVE_XCM        =       no, libXcm not found"
else
echo "HAVE_XCM        =       yes (use libXcm)"
fi
if [[ "$HAVE_X11" = "" ]]; then
echo "HAVE_X11        =       no, X Color Management support skipped"
else
echo "HAVE_X11        =       yes (X Color Management)"
fi
if [[ "$HAVE_LINUX" = "" ]]; then
echo "HAVE_LINUX      =       no, DDC over i2c support skipped"
else
echo "HAVE_LINUX      =       yes (DDC over i2c)"
fi
if [[ "$HAVE_OY" = "" ]]; then
echo "HAVE_OY         =       no, Oyranos not used"
else
echo "HAVE_OY         =       yes (use Oyranos)"
fi
echo "CFLAGS          =       $CFLAGS"
echo "CXXFLAGS        =       $CXXFLAGS"
echo "LDFLAGS         =       $LDFLAGS"
echo "prefix          =       $prefix"
echo "exec_prefix     =       $exec_prefix"
echo "bindir          =       $bindir"
echo "sbindir         =       $sbindir"
echo "libdir          =       $libdir"
echo "includedir      =       $includedir"
echo "datadir         =       $datadir"
echo "mandir          =       $mandir"
echo "udevdir         =       $udevdir"
echo "pixmapdir       =       $pixmapdir"
echo "desktopdir      =       $desktopdir"
