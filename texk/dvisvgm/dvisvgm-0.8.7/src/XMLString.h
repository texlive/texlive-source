/*************************************************************************
** XMLString.h                                                          **
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

#ifndef XMLSTRING_H
#define XMLSTRING_H

#include <ostream>
#include <string>
#include <sstream>
#include "macros.h"

using std::string;
using std::ostream;
using std::stringstream;

class XMLString : public string
{
   public:
		XMLString () : string() {}
		XMLString (const char *str, bool plain=false);
      XMLString (const string &str, bool plain=false);
		XMLString (int n, bool cast=true);
		XMLString (double x);
//		ostream& write (ostream &os) const;
};


//IMPLEMENT_OUTPUT_OPERATOR(XMLString)

#endif
