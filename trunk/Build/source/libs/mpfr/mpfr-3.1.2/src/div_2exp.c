/* mpfr_div_2exp -- divide a floating-point number by a power of two

Copyright 1999, 2001, 2004, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.
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

/* Obsolete function, use mpfr_div_2ui or mpfr_div_2si instead. */

#undef mpfr_div_2exp

int
mpfr_div_2exp (mpfr_ptr y, mpfr_srcptr x, unsigned long int n, mpfr_rnd_t rnd_mode)
{
  return mpfr_div_2ui (y, x, n, rnd_mode);
}
