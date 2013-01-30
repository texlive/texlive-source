/*************************************************************************
** MiKTeXCom.h                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef MIKTEXCOM_H
#define MIKTEXCOM_H

#include <string>

#ifdef _MSC_VER
#import <MiKTeX209-session.tlb>
using namespace MiKTeXSession2_9;
#else
#include "miktex209-session.h"
#endif


class MiKTeXCom
{
	public:
		MiKTeXCom ();
		~MiKTeXCom ();
		std::string getVersion ();
		const char* findFile (const char* fname);

	private:
#ifdef _MSC_VER
		ISession2Ptr _session;
#else
		ISession2 *_session;
#endif
};


#endif
