AC_INIT([fap2-studio-gtk], [0.1.0], [yuri.borisov.v@gmail.com])

AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_FILES([
		 Makefile
		 src/Makefile
		 ])


AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

CFLAGS="-g -O0"
#CFLAGS="-g -O2"

dnl Check for Glib/GConf
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.0.0], , [AC_MSG_ERROR([
You must have the GLib 2.0 and development headers installed to build.
])])

AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)


PKG_CHECK_MODULES(GTKMM, [gtkmm-2.4 >= 2.8.0], , [
    AC_MSG_ERROR([You must have 'libgtkmm-2.4' dev headers (libgtkmm-2.4-dev) installed
    ]) ])

AC_SUBST(GTKMM_CFLAGS)
AC_SUBST(GTKMM_LIBS)


PKG_CHECK_MODULES(FAPWS2, [libgrayb >= 0.1], , [
    AC_MSG_ERROR([You must have 'libgrayb' dev headers (libgrayb-dev) installed
    ]) ])

AC_SUBST(FAPWS2_CFLAGS)
AC_SUBST(FAPWS2_LIBS)




 
CXXFLAGS="$CFLAGS $GLIB_CFLAGS $GTKMM_CFLAGS $FAPWS2_CFLAGS" 
LDFLAGS="$LDFLAGS $GLIB_LIBS $GTKMM_LIBS $FAPWS2_LIBS"

AC_OUTPUT

echo Environment variables
echo CFLAGS $CFLAGS

