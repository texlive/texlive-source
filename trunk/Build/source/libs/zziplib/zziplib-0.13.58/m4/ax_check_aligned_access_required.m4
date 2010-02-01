dnl /usr/share/aclocal/guidod-cvs/ax_check_aligned_access_required.m4
dnl @synopsis AC_CHECK_ALIGNED_ACCESS_REQUIRED
dnl
dnl While the x86 CPUs allow access to memory objects to be unaligned
dnl it happens that most of the modern designs require objects to be
dnl aligned - or they will fail with a buserror. That mode is quite known
dnl by big-endian machines (sparc, etc) however the alpha cpu is little-
dnl endian.
dnl
dnl The following function will test for aligned access to be required and
dnl set a config.h define HAVE_ALIGNED_ACCESS_REQUIRED (name derived by
dnl standard usage). Structures loaded from a file (or mmapped to memory)
dnl should be accessed per-byte in that case to avoid segfault type errors.
dnl
dnl @category C
dnl @author Guido U. Draheim <guidod@gmx.de>
dnl @version 2009-05-22
dnl @license GPLWithACException

AC_DEFUN([AX_CHECK_ALIGNED_ACCESS_REQUIRED],
[AC_CACHE_CHECK([if pointers to integers require aligned access],
  [ax_cv_have_aligned_access_required],
[if test "$cross_compiling" = "yes"; then
  case "$host_cpu" in alpha*|arm*|bfin*|hp*|mips*|sh*|sparc*|ia64|nv1)
    ax_cv_have_aligned_access_required="yes"
  ;; esac
else
  AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>

int main()
{
  char* string = malloc(40);
  int i;
  for (i=0; i < 40; i++) string[[i]] = i;
  {
     void* s = string;
     int* p = s+1;
     int* q = s+2;

     if (*p == *q) { return 1; }
  }
  return 0;
}
              ],
     [ax_cv_have_aligned_access_required=yes],
     [ax_cv_have_aligned_access_required=no],
     [ax_cv_have_aligned_access_required=no])
  ])
fi
if test "$ax_cv_have_aligned_access_required" = yes ; then
  AC_DEFINE([HAVE_ALIGNED_ACCESS_REQUIRED], [1],
    [Define if pointers to integers require aligned access])
fi
])

