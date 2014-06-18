/* mpfr_set_default_rounding_mode -- set the default rounding mode
   mpfr_get_default_rounding_mode -- get the default rounding mode

Copyright 1999, 2001, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.
Contributed by the AriC and Caramel projects, INRIA.

This file is part of the GNU MPFR Library.

The GNU MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MPFR Library; see the file COPYING.LESSER.  If not, see
http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

#include "mpfr-impl.h"

mpfr_rnd_t MPFR_THREAD_ATTR __gmpfr_default_rounding_mode = MPFR_RNDN;

void
mpfr_set_default_rounding_mode (mpfr_rnd_t rnd_mode)
{
  if (rnd_mode >= MPFR_RNDN && rnd_mode < MPFR_RND_MAX)
    __gmpfr_default_rounding_mode = rnd_mode;
}

#undef mpfr_get_default_rounding_mode
mpfr_rnd_t
mpfr_get_default_rounding_mode (void)
{
  return __gmpfr_default_rounding_mode;
}
