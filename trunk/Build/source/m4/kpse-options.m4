# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_OPTIONS
# ------------
# Provide configure options used by the higher level TL directories
# as well as by packages maintained independently.
AC_DEFUN([KPSE_OPTIONS],
[AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_ARG_ENABLE([multiplatform],
              AS_HELP_STRING([--enable-multiplatform],
                             [put executables into bin/PLATFORM and libraries into lib/PLATFORM]))[]dnl
if test "x$enable_multiplatform" = xyes; then
  if test "x$bindir" = 'x${exec_prefix}/bin'; then
    bindir="$bindir/$host"
    ac_configure_args="$ac_configure_args '--bindir=$bindir'"
  fi
  if test "x$libdir" = 'x${exec_prefix}/lib'; then
    libdir="$libdir/$host"
    ac_configure_args="$ac_configure_args '--libdir=$libdir'"
  fi
fi
]) # KPSE_OPTIONS
