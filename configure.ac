AC_INIT([RPhoto],[0.4.5])
AC_CONFIG_SRCDIR([configure.ac])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL

	
AC_ARG_ENABLE(debug,
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) 
    DEBUGG="-g -fomit-frame-pointer -DDEBUG" 
    WXDEBUG="--debug"
    ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=false])


dnl wxWindows -----
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.3.2, wxWin=1, , , "$WXDEBUG")
        if test "$wxWin" != 1; then
        AC_MSG_ERROR([
                wxWindows must be installed on your system.

                Please check that wx-config is in path, the directory
                where wxWindows libraries are installed (returned by
                'wx-config --libs' or 'wx-config --static --libs' command)
                is in LD_LIBRARY_PATH or equivalent variable and
                wxWindows version is 2.3.2 or above.
                ])
        fi

AC_CHECK_LIB(jpeg, main)
PKG_CHECK_MODULES(EXIF, libexif,,AC_MSG_ERROR([libexif not found!]))
# AC_CHECK_LIB(exif, main)

LIBS="$DEBUGG $WX_LIBS $EXIF_LIBS -ljpeg"
CXXFLAGS="$DEBUGG  $WX_CXXFLAGS $EXIF_CXXFLAGS"
CPPFLAGS="$DEBUGG $WX_CPPFLAGS $EXIF_CPPFLAGS"
CFLAGS="$DEBUGG $WX_CFLAGS $EXIF_CFLAGS"
dnl --------------

dnl GetTest stuff
dnl ALL_LINGUAS="fr"
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

dnl wxVillaLib compatibility
GNOMEUI2_CFLAGS=
AC_SUBST(GNOMEUI2_CFLAGS)

dnl Binaries path
CPPFLAGS="$CPPFLAGS -DBINDIR=\"$bindir\""

AC_CONFIG_FILES([ 
    Makefile \
    src/Makefile \
    lib/Makefile \
    lib/wxFix/Makefile \
    lib/wxVillaLib/Makefile \
    lib/wxmisc/Makefile \
    lib/wxRectTracker/Makefile \
    lib/wxJPEGExifHandler/Makefile \
	po/Makefile.in \
    doc/Makefile \
    doc/use/Makefile \
    resources/Makefile \
    resources/xpm/Makefile \
    build/Makefile \
    build/linux/Makefile \
    ])
dnl AC_CONFIG_FILES([po/Makefile], [AM_POSTPROCESS_PO_MAKEFILE])
AC_OUTPUT
