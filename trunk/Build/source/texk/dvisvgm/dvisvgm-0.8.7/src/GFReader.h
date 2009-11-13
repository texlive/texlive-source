/*************************************************************************
** GFReader.h                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef GFREADER_H
#define GFREADER_H

#include <istream>
#include <map>
#include <string>
#include <vector>
#include "Bitmap.h"
#include "types.h"

using std::istream;
using std::map;
using std::string;
using std::vector;

class CharInfo;

class GFReader
{
	typedef map<UInt8,CharInfo*>::iterator Iterator;
	typedef map<UInt8,CharInfo*>::const_iterator ConstIterator;
   public:
      GFReader (istream &is);
      virtual ~GFReader ();
		bool executeChar (UInt8 c);
		bool executeAllChars ();
		bool executePostamble ();
		virtual void preamble (const string &str) {}
		virtual void postamble () {}
		virtual void beginChar (UInt32 c) {}
		virtual void endChar (UInt32 c) {}
		virtual void special (string str) {}
		virtual void numspecial (Int32 y) {}
		const Bitmap& getBitmap () const {return bitmap;}
		double getDesignSize () const;
		double getHPixelsPerPoint () const;
		double getVPixelsPerPoint () const;
		double getCharWidth (UInt32 c) const;
		UInt32 getChecksum () const  {return checksum;}

	protected:
		Int32 readSigned (int bytes);
		UInt32 readUnsigned (int bytes);
		string readString (int len);
		int executeCommand ();
		istream& getInputStream () const {return in;}

		void cmdPre (int);
		void cmdPost (int);
		void cmdPostPost (int);
		void cmdPaint0 (int pixels);
		void cmdPaint (int len);
		void cmdBoc (int);
		void cmdBoc1 (int);
		void cmdEoc (int);
		void cmdSkip (int len);
		void cmdNewRow (int col);
		void cmdXXX (int len);
		void cmdYYY (int);
		void cmdNop (int);
		void cmdCharLoc0 (int);
		void cmdCharLoc (int);

   private:
		istream &in;
		Int32 minX, maxX, minY, maxY;
		Int32 x, y;            // current pen location (pixel units)
		Int32 currentChar;
		Bitmap bitmap;         // bitmap of current char
		FixWord designSize;    // designSize
		ScaledInt hppp, vppp;  // horizontal and vertical pixel per point
		UInt32 checksum;
		map<UInt8,CharInfo*> charInfoMap;
		bool penDown;
		bool valid;
};

#endif
