# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2002 Olaf Weber <olaf@infovore.xs4all.nl>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_ENABLE_CXX_HACK
# --------------------
# Provide the configure option '--enable-cxx-runtime-hack'.
AC_DEFUN([KPSE_ENABLE_CXX_HACK],
[AC_ARG_ENABLE([cxx-runtime-hack],
               AS_HELP_STRING([--enable-cxx-runtime-hack],
                              [link C++ runtime statically],
                              [29]))[]dnl
]) # KPSE_ENABLE_CXX_HACK

# KPSE_CXX_HACK()
# ---------------
# Try statically linking C++ runtime library (g++ only)
# and define CXX_HACK_LIBS with required flags.
# Since Libtool reshuffles the argument list, we define CXXLD
# as wrapper script appending these flags to g++ as invoked
# by Libtool when linking progams (but not shared libraries).
#
AC_DEFUN([KPSE_CXX_HACK],
[AC_REQUIRE([AC_PROG_CXX])[]dnl
AC_REQUIRE([KPSE_ENABLE_CXX_HACK])[]dnl
if test "x$GXX:$enable_cxx_runtime_hack" = xyes:yes; then
  _KPSE_CXX_HACK
fi
if test "x$kpse_cv_cxx_hack" = xok; then
  CXXLD='$(top_builddir)/CXXLD.sh'
  CXX_HACK_DEPS=$CXXLD
  CXX_HACK_LIBS=$kpse_cv_cxx_flags
  cxxld_sh="#! ${CONFIG_SHELL-/bin/sh}
# CXXLD.sh.  Generated by configure.
set -- $CXX \"\$[]@\" $CXX_HACK_LIBS
echo \"\$[]0:\" \"\$[]@\"
exec \"\$[]@\""
  AC_CONFIG_FILES([CXXLD.sh:Makefile.in],
                  [echo "$cxxld_sh" >CXXLD.sh; chmod +x CXXLD.sh],
                  [cxxld_sh='$cxxld_sh'])
else
  CXXLD='$(CXX)'
fi
AC_SUBST([CXXLD])
AC_SUBST([CXX_HACK_DEPS])
AC_SUBST([CXX_HACK_LIBS])
]) # KPSE_CXX_HACK

# _KPSE_CXX_HACK()
# ----------------
# internal subroutine
m4_define([_KPSE_CXX_HACK], [
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([iostream])
AC_CACHE_CHECK([for statically linking C++ runtime library],
               [kpse_cv_cxx_hack],
  [AC_LANG_CONFTEST([AC_LANG_PROGRAM([[#ifdef HAVE_IOSTREAM
#include <iostream>
using namespace std;
#else
#include <iostream.h>
#endif]],
                                     [[cout <<"worksok\n";]])])
  flags_try0='-nodefaultlibs -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic -lm'
  flags_try1='-lgcc_eh -lgcc -lc -lgcc_eh -lgcc'
  flags_try2='-lgcc -lc -lgcc'
  kpse_save_LIBS=$LIBS
  cpp_link_hack=false
  for flags in "$flags_try1" "$flags_try2"; do
    LIBS="$kpse_save_LIBS $flags_try0 $flags"
    AC_LINK_IFELSE([],
                   [AS_IF([test "x$cross_compiling" = xyes],
                          [cpp_link_hack=true; break],
                          [AS_CASE([`(./conftest$ac_exeext; exit) 2>/dev/null`],
                                   [worksok], [cpp_link_hack=true; break])])])
  done
  LIBS=$kpse_save_LIBS
  if $cpp_link_hack; then
    kpse_cv_cxx_hack=ok
    kpse_cv_cxx_flags="$flags_try0 $flags"
  else
    kpse_cv_cxx_hack="not supported"
  fi])
if test "x$kpse_cv_cxx_hack" = xok; then
  AC_MSG_NOTICE([using $kpse_cv_cxx_flags])
fi
AC_LANG_POP([C++])
]) # _KPSE_CXX_HACK
