/*************************************************************************
** MetafontWrapper.h                                                    **
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

#ifndef METAFONTWRAPPER_H
#define METAFONTWRAPPER_H

#include <string>

using std::string;

class FileFinder;

class MetafontWrapper
{
   public:
      MetafontWrapper (const string &fontname);
		int call (const string &mode, double mag);
		int make (const string &mode, double mag);
		bool success () const;
		void removeOutputFiles (bool keepGF=false);
		static void removeOutputFiles (const string &fontname, bool keepGF=false);

   private:
		string _fontname;
};

#endif
