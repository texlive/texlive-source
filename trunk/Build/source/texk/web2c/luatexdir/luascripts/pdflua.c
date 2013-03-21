/* pdflua.c

   Copyright 2010 Taco Hoekwater <taco@luatex.org>
   Copyright 2010 Hartmut Henkel <hartmut@luatex.org>

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

#include "ptexlib.h"

static const Byte compr[250] = {
    0x78, 0x9c, 0xad, 0x51, 0xc1, 0x4a, 0xc4, 0x30, 0x14, 0xbc, 0xbf, 0xaf, 0x78, 0x87, 0x1e, 0x14,
    0xb6, 0x49, 0xba, 0x0a, 0xd2, 0x82, 0x1f, 0x20, 0x78, 0x12, 0xbc, 0x78, 0xeb, 0xa6, 0xaf, 0x6d,
    0x68, 0x48, 0xc2, 0x4b, 0xe2, 0xba, 0x88, 0xff, 0x6e, 0x0a, 0x45, 0x57, 0xc5, 0xdb, 0x86, 0x97,
    0x30, 0xcc, 0x4c, 0x32, 0x03, 0xa9, 0x6b, 0xac, 0x1e, 0x86, 0x0e, 0xc3, 0x30, 0xda, 0xdc, 0x8b,
    0xb2, 0xf1, 0xa6, 0xbd, 0x6d, 0x71, 0xaf, 0x1a, 0x55, 0x37, 0x4d, 0xad, 0xee, 0x50, 0xa9, 0x4e,
    0xb5, 0x5d, 0xd3, 0xbe, 0xe0, 0x3c, 0x93, 0x5b, 0xc8, 0x62, 0x05, 0x75, 0xb9, 0xf6, 0xfc, 0xf4,
    0xd8, 0xe1, 0x9c, 0x52, 0xe8, 0xa4, 0x1c, 0x7d, 0x76, 0x03, 0x9f, 0x44, 0xcc, 0x81, 0x2c, 0x69,
    0x31, 0xb2, 0x8c, 0xaf, 0x4e, 0x96, 0xe7, 0x12, 0xbd, 0xc9, 0xc4, 0xd9, 0x2d, 0x32, 0xfa, 0xcc,
    0x9a, 0x64, 0x21, 0x16, 0x79, 0xa4, 0xc3, 0x5e, 0x6f, 0xf2, 0x60, 0x78, 0x45, 0x51, 0xb3, 0x09,
    0x29, 0xca, 0xb3, 0x26, 0x15, 0xac, 0x41, 0x69, 0x36, 0x11, 0xcb, 0x50, 0xcf, 0xf6, 0x84, 0x47,
    0xcf, 0x0b, 0x1a, 0x87, 0x81, 0xfd, 0xc4, 0x14, 0xa3, 0x10, 0x62, 0x75, 0x5d, 0x66, 0x01, 0x1c,
    0x68, 0x32, 0x2e, 0xf4, 0x13, 0xe1, 0x3d, 0x8e, 0xd9, 0xe9, 0x64, 0xbc, 0xbb, 0xea, 0xaf, 0x81,
    0xdc, 0x00, 0xeb, 0xf1, 0x9f, 0xe4, 0x73, 0x0a, 0x65, 0x8a, 0x1a, 0x13, 0xd3, 0x0f, 0xcb, 0xe6,
    0xb8, 0x5c, 0x47, 0xeb, 0x75, 0x6f, 0xb7, 0x1f, 0x2b, 0x41, 0xef, 0x80, 0x78, 0x5e, 0xfb, 0x0b,
    0xef, 0x8a, 0xf0, 0x5d, 0x79, 0x43, 0x2b, 0xf9, 0xb7, 0xec, 0x2f, 0x66, 0x07, 0x1f, 0x00, 0x4c,
    0x29, 0xb3, 0xdb, 0x72, 0xe0, 0x13, 0x6d, 0xe3, 0xa0, 0x0e
};

static const zlib_struct compr_struct = { 553, 250, compr };

const zlib_struct *pdflua_zlib_struct_ptr = &compr_struct;
