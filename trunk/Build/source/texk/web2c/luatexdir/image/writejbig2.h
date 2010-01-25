/* writejbig2.h

   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2009 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

/* $Id: writejbig2.h 2327 2009-04-18 12:47:21Z hhenkel $ */

#ifndef WRITEJBIG2_H
#  define WRITEJBIG2_H

#  include "image.h"

void flush_jbig2_page0_objects(void);
void read_jbig2_info(image_dict *);
void write_jbig2(image_dict *);

#endif                          /* WRITEJBIG"_H */
