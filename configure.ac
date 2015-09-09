#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([p4ofagent], [0.1], [piercemcentagart@barefootnetworks.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/p4ofagent.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
CFLAGS="-g -Wall -Wextra -Wno-unused-parameter"
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h stdint.h stdlib.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memset socket])

# Generate makefiles
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

