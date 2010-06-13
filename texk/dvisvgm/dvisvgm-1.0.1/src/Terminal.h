/*************************************************************************
** Terminal.h                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef TERMINAL_H
#define TERMINAL_H

#include <ostream>

class Terminal
{
	public:
		static const int DEFAULT;
		static const int BLACK;
		static const int RED;
		static const int GREEN;
		static const int BLUE;
		static const int CYAN;
		static const int YELLOW;
		static const int MAGENTA;
		static const int WHITE;

	public:
		static void init (std::ostream &os);
		static void finish (std::ostream &os);
		static int columns ();
		static int rows ();
		static void color (int color, bool light, std::ostream &os);
		static void bgcolor (int color, bool light, std::ostream &os);

#ifdef __WIN32__
	private:
		static int _defaultColor;
#endif
};

#endif
