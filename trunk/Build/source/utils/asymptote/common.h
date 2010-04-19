/****
 * common.h
 *
 * Definitions common to all files.
 *****/

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <climits>

#ifdef __CYGWIN__
#undef LONG_LONG_MAX
#define LONG_LONG_MAX __LONG_LONG_MAX__
#undef LONG_LONG_MIN
#define LONG_LONG_MIN (-LONG_LONG_MAX-1)
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_LIBGL) && defined(HAVE_LIBGLU) && defined(HAVE_LIBGLUT)
#define HAVE_GL
#endif

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include "memory.h"

#if defined(HAVE_LONG_LONG) && defined(LONG_LONG_MAX) && defined(LONG_LONG_MIN)
#define Int_MAX LONG_LONG_MAX
#define Int_MIN LONG_LONG_MIN
typedef long long Int;
typedef unsigned long long unsignedInt;
#else
#undef HAVE_LONG_LONG
#ifdef HAVE_LONG
#define Int_MAX LONG_MAX
#define Int_MIN LONG_MIN
typedef long Int;
typedef unsigned long unsignedInt;
#else
#define Int_MAX INT_MAX
#define Int_MIN INT_MIN
typedef int Int;
typedef unsigned int unsignedInt;
#endif
#endif

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::istream;
using std::ostream;

using mem::string;
using mem::stringstream;
using mem::istringstream;
using mem::ostringstream;
using mem::stringbuf;

#endif 
