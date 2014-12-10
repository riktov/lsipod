#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lsipod], [0.1], [riktov@freeshell.de])
AC_CONFIG_SRCDIR([lsipod.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([gpod], itdb_filename_ipod2fs)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

PKG_CHECK_MODULES(libgpod, [libgpod-1.0])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([strdup])

CFLAGS="$libgpod_CFLAGS $CFLAGS"
LIBS="$libgpod_LIBS $LIBS"
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
