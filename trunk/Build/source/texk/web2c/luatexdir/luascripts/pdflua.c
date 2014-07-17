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

static const Byte compr[260] = {
    0x78, 0x9c, 0xad, 0x51, 0xc1, 0x4a, 0xc4, 0x30, 0x10, 0xbd, 0xe7, 0x2b, 0xe6, 0xd0, 0x83, 0xc2,
    0x36, 0x49, 0x57, 0x41, 0x5a, 0xd8, 0x0f, 0x10, 0x3c, 0x09, 0x5e, 0xbc, 0xa5, 0xe9, 0xb4, 0x0d,
    0x0d, 0x49, 0x98, 0x24, 0xee, 0x2e, 0xe2, 0xbf, 0x9b, 0x42, 0xd1, 0x55, 0xf1, 0xb6, 0x61, 0x12,
    0x1e, 0xf3, 0x5e, 0xe6, 0x3d, 0x98, 0xba, 0x86, 0xea, 0x71, 0xe8, 0x20, 0x0c, 0xa3, 0xcd, 0x8a,
    0x97, 0x0b, 0x77, 0xed, 0x7d, 0x0b, 0x7b, 0xd9, 0xc8, 0xba, 0x69, 0x6a, 0xf9, 0x00, 0x52, 0x76,
    0xb2, 0xed, 0x9a, 0xf6, 0x15, 0xe6, 0x19, 0xdd, 0x82, 0x16, 0x2a, 0x56, 0x97, 0x6f, 0x2f, 0xcf,
    0x4f, 0x1d, 0xcc, 0x29, 0x85, 0xd8, 0x09, 0x31, 0xfa, 0xec, 0x06, 0x3a, 0xf3, 0x98, 0x03, 0x5a,
    0xd4, 0x7c, 0x24, 0x11, 0xdf, 0x9c, 0x28, 0xf3, 0x12, 0x9e, 0x44, 0x4f, 0xca, 0xe9, 0x19, 0xa3,
    0xc0, 0x53, 0x3d, 0xd9, 0x73, 0x98, 0x45, 0xf4, 0x99, 0x34, 0x8a, 0x42, 0x2e, 0xe2, 0x88, 0xfd,
    0x5e, 0x6f, 0xd2, 0xc1, 0xd0, 0x8a, 0xa2, 0x26, 0x13, 0x52, 0x14, 0x17, 0xb1, 0x2a, 0xb6, 0xba,
    0xa6, 0xd9, 0x44, 0x28, 0x85, 0x8a, 0xec, 0x19, 0x8e, 0x9e, 0x16, 0x30, 0x0e, 0x02, 0xf9, 0x89,
    0x30, 0x46, 0xce, 0xf9, 0xaa, 0xba, 0xce, 0x61, 0xac, 0xc7, 0xc9, 0xb8, 0xa0, 0x26, 0x84, 0x03,
    0x8c, 0xd9, 0xe9, 0x64, 0xbc, 0xbb, 0x51, 0xb7, 0x0c, 0xdd, 0xc0, 0xd6, 0xe7, 0x3f, 0xca, 0xe7,
    0x14, 0x4a, 0x15, 0x36, 0x26, 0xc2, 0x1f, 0x92, 0x4d, 0x71, 0xbd, 0x8c, 0xd6, 0x6b, 0x65, 0xb7,
    0xf5, 0x15, 0xa3, 0x77, 0x06, 0x70, 0x19, 0xfb, 0x0b, 0xef, 0x0a, 0xf1, 0x1d, 0x79, 0x43, 0x6b,
    0xf3, 0x6f, 0xd8, 0x5f, 0x9d, 0x1d, 0xfb, 0x60, 0x8c, 0x30, 0x65, 0x72, 0x9b, 0x0f, 0xfb, 0x04,
    0xb9, 0x50, 0xa4, 0xf0
};

static const zlib_struct compr_struct = { 566, 260, compr };

const zlib_struct *pdflua_zlib_struct_ptr = &compr_struct;