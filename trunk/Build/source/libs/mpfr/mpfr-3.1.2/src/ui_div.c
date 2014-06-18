/* mpfr_ui_div -- divide a machine integer by a floating-point number
   mpfr_si_div -- divide a machine number by a floating-point number

Copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.
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


#define MPFR_NEED_LONGLONG_H
#include "mpfr-impl.h"

int
mpfr_ui_div (mpfr_ptr y, unsigned long int u, mpfr_srcptr x, mpfr_rnd_t rnd_mode)
{
  mpfr_t uu;
  mp_limb_t up[1];
  unsigned long cnt;

  MPFR_LOG_FUNC
    (("u=%lu x[%Pu]=%.*Rg rnd=%d",
      u, mpfr_get_prec(x), mpfr_log_prec, x, rnd_mode),
     ("y[%Pu]=%.*Rg", mpfr_get_prec(y), mpfr_log_prec, y));

  if (MPFR_UNLIKELY(MPFR_IS_SINGULAR(x)))
    {
      if (MPFR_IS_NAN(x))
        {
          MPFR_SET_NAN(y);
          MPFR_RET_NAN;
        }
      else if (MPFR_IS_INF(x)) /* u/Inf = 0 */
        {
          MPFR_SET_ZERO(y);
          MPFR_SET_SAME_SIGN(y,x);
          MPFR_RET(0);
        }
      else /* u / 0 */
        {
          MPFR_ASSERTD(MPFR_IS_ZERO(x));
          if (u)
            {
              /* u > 0, so y = sign(x) * Inf */
              MPFR_SET_SAME_SIGN(y, x);
              MPFR_SET_INF(y);
              mpfr_set_divby0 ();
              MPFR_RET(0);
            }
          else
            {
              /* 0 / 0 */
              MPFR_SET_NAN(y);
              MPFR_RET_NAN;
            }
        }
    }
  else if (MPFR_LIKELY(u != 0))
    {
      MPFR_TMP_INIT1(up, uu, GMP_NUMB_BITS);
      MPFR_ASSERTN(u == (mp_limb_t) u);
      count_leading_zeros(cnt, (mp_limb_t) u);
      up[0] = (mp_limb_t) u << cnt;
      MPFR_SET_EXP (uu, GMP_NUMB_BITS - cnt);
      return mpfr_div (y, uu, x, rnd_mode);
    }
  else /* u = 0, and x != 0 */
    {
      MPFR_SET_ZERO(y);         /* if u=0, then set y to 0 */
      MPFR_SET_SAME_SIGN(y, x); /* u considered as +0: sign(+0/x) = sign(x) */
      MPFR_RET(0);
    }
}


int
mpfr_si_div (mpfr_ptr y, long int u, mpfr_srcptr x, mpfr_rnd_t rnd_mode)
{
  int res;

  if (u >= 0)
    res = mpfr_ui_div (y, u, x, rnd_mode);
  else
    {
      res = -mpfr_ui_div (y, -u, x, MPFR_INVERT_RND(rnd_mode));
      MPFR_CHANGE_SIGN (y);
    }
  return res;
}
