/*************************************************************************
** SVGFontTraceEmitter.h                                                **
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

#ifndef SVGFONTTRACEREMITTER_H
#define SVGFONTTRACEREMITTER_H

#include <istream>
#include <set>
#include "CharmapTranslator.h"
#include "FontEmitter.h"
#include "XMLNode.h"


class FileFinder;
class Font;
class FontCache;
class FontManager;
class GFGlyphTracer;
class SVGTree;
class XMLElementNode;

class SVGFontTraceEmitter : public FontEmitter
{
   public:
      SVGFontTraceEmitter (const Font *font, const FontManager &fm, const CharmapTranslator &cmt, SVGTree &svg, bool uf);
      ~SVGFontTraceEmitter ();
		int emitFont (const char *id);
		int emitFont (const std::set<int> &usedChars, const char *id);
		bool emitGlyph (int c);

	public:
		static const char *CACHE_PATH; ///< path to cache directory (0 if caching is disabled)
		static bool TRACE_ALL;         ///< if true, not only the actually used, but all font glyphs are traced
		static double METAFONT_MAG;    ///< magnification factor for Metafont calls

	protected:
		int emitFont (const std::set<int> *usedChars, const char *id);
		bool prepareTracer ();
		void traceAllGlyphs ();

   private:
		GFGlyphTracer *_gfTracer;
		std::istream *_in;
		const Font *_font;
		const FontManager &_fontManager;
		FontCache *_cache;
		const CharmapTranslator &_charmapTranslator;
		SVGTree &_svg;
		XMLElementNode *_glyphNode;
		bool _useFonts;
};

#endif
