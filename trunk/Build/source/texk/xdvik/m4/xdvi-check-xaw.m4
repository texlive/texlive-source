# Autoconf macros for xdvik.
# Copyright (C) 2003 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_CHECK_XAW_HEADERS
# ----------------------
# Check for Xaw headers and library version.
AC_DEFUN([XDVI_CHECK_XAW_HEADERS],
[save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_MSG_CHECKING([for Xaw headers])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Form.h>]])],
                  [xdvi_have_xaw=yes],
                  [xdvi_have_xaw=no])
AC_MSG_RESULT([$xdvi_have_xaw])
#
if test "x$xdvi_have_xaw" = xyes; then
 _XDVI_CHECK_XAW_VERSION
fi
CPPFLAGS=$save_CPPFLAGS
]) # XDVI_CHECK_XAW_HEADERS

# _XDVI_CHECK_XAW_VERSION
# -----------------------
# Check Xaw version.
m4_define([_XDVI_CHECK_XAW_VERSION],
[AC_CHECK_MEMBER([SimpleClassPart.extension],
                 [],
                 [AC_DEFINE([HAVE_OLD_XAW], 1,
                            [Define if you have an old version of the Xaw library])],
                 [[
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/SimpleP.h>]])
]) # _XDVI_CHECK_XAW_VERSION

# XDVI_CHECK_XAW_LIBRARY
# ----------------------
# Check for Xaw library.
# If found, set prog_extension and x_tool_libs, and define XAW.
AC_DEFUN([XDVI_CHECK_XAW_LIBRARY],
[AC_REQUIRE([XDVI_CHECK_XAW_HEADERS])
if test "x$xdvi_have_xaw" = xyes; then
  :
fi 
if test "x$xdvi_have_xaw" = xyes; then
  prog_extension="xaw"
  x_tool_libs="-lXaw"
  AC_DEFINE([XAW], 1, [Define to use the Xaw toolkit.])
else
  AC_MSG_ERROR([Sorry, you will need at least the Xaw header/library files to compile xdvik.])
fi
]) # XDVI_CHECK_XAW_LIBRARY
