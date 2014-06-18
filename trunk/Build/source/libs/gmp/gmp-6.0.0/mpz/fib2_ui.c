/* mpz_fib2_ui -- calculate Fibonacci numbers.

Copyright 2001, 2012 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"


void
mpz_fib2_ui (mpz_ptr fn, mpz_ptr fnsub1, unsigned long n)
{
  mp_ptr     fp, f1p;
  mp_size_t  size;

  size = MPN_FIB2_SIZE (n);
  fp =  MPZ_REALLOC (fn,     size);
  f1p = MPZ_REALLOC (fnsub1, size);

  size = mpn_fib2_ui (fp, f1p, n);

  SIZ(fn)     = size - (n == 0);
  SIZ(fnsub1) = size - (f1p[size-1] == 0);
}
