/*************************************************************************
** CRC32.h                                                              **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2011 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#ifndef CRC32_H
#define CRC32_H

#include <cstdlib>
#include <istream>
#include "types.h"

class CRC32
{
   public:
      CRC32 ();
		void update (const UInt8 *bytes, size_t len);
		void update (UInt32 n, int bytes=4);
		void update (const char *str);
		void update (std::istream &is);
		UInt32 get () const;
		void reset ();
		static UInt32 compute (const UInt8 *bytes, size_t len);
		static UInt32 compute (const char *str);
		static UInt32 compute (std::istream &is);

	protected:
		CRC32 (const CRC32 &crc32) {}

   private:
		UInt32 _crc32;
		UInt32 _tab[256];
};


#endif
