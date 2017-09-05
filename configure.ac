#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([HexIt], [1.8], [timb@nth-dimension.org.uk])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/HexIt.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP

AC_LANG([C])

# Checks for pkg-conf.

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h errno.h unistd.h sys/types.h sys/stat.h fcntl.h string.h sys/mman.h ctype.h],, AC_MSG_ERROR([required header file missing]))

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([fprintf exit perror getopt atol atoi open sysconf fstat strlen mmap isalnum munmap printf close],, AC_MSG_ERROR([required standard library function missing]))

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile man/Makefile])
AC_OUTPUT
