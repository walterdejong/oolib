
AC_PREREQ([2.61])
AC_INIT([oolib], [0.1], [walter@heiho.net])
AC_CONFIG_AUX_DIR(aux-dist)
AM_INIT_AUTOMAKE

AC_PROG_MAKE_SET
AC_PROG_CXX
AC_LANG(C++)

CXXFLAGS="-std=c++11 -fPIC"

AC_CONFIG_MACRO_DIR([m4])

LT_PREREQ([2.2])
LT_INIT([dlopen shared])

AC_CONFIG_FILES([Makefile include/Makefile src/Makefile])

dnl check for pcre header files
AC_CHECK_HEADERS([pcre.h],
	[oolib_found_pcre_headers=yes; break;])
AS_IF([test "x$oolib_found_pcre_headers" != "xyes"],
	[AC_MSG_ERROR([Unable to find the pcre headers])])

dnl check for pcre library
SAVED_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -lpcre"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <pcre.h>],
    [])],
  [TEST_LIBS="$TEST_LIBS -lpcre"] [HAVE_PCRE=1],
  [AC_MSG_WARN([libpcre is not installed.])])
LDFLAGS=$SAVED_LDFLAGS

AC_OUTPUT

