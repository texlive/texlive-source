/*************************************************************************
** FontCache.cpp                                                        **
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

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "FileSystem.h"
#include "FontCache.h"
#include "FontGlyph.h"
//#include "gzstream.h"
#include "types.h"

using namespace std;


static UInt32 read_unsigned (int bytes, istream &is) {
	UInt32 ret = 0;
	for (bytes--; bytes >= 0 && !is.eof(); bytes--) {
		UInt32 b = is.get();
		ret |= b << (8*bytes);
	}
	return ret;
}


static Int32 read_signed (int bytes, istream &is) {
	Int32 ret = is.get();
	if (ret & 128)        // negative value?
		ret |= 0xffffff00;
	for (bytes-=2; bytes >= 0 && !is.eof(); bytes--)
		ret = (ret << 8) | is.get();
	return ret;
}


static void write_unsigned (UInt32 value, int bytes, ostream &os) {
	for (bytes--; bytes >= 0; bytes--)
		os.put((value >> (8*bytes)) & 0xff);
}


static inline void write_signed (Int32 value, int bytes, ostream &os) {
	write_unsigned((UInt32)value, bytes, os);
}


static LPair read_pair (int bytes, istream &is) {
	long x = read_signed(bytes, is);
	long y = read_signed(bytes, is);
	return LPair(x, y);
}


FontCache::FontCache () : _changed(false)
{
}


FontCache::~FontCache () {
	clear();
}


/** Removes all data from the cache. This does not affect the cache files. */
void FontCache::clear () {
	FORALL(_glyphs, GlyphMap::iterator, it) {
		delete it->second;
	}
	_glyphs.clear();
}


/** Assigns glyph data to a character and adds it to the cache.
 *  @param[in] c character code
 *  @param[in] glyph font glyph data */
void FontCache::setGlyph (int c, const Glyph *glyph) {
	if (!glyph || glyph->empty())
		delete glyph;
	else {
		GlyphMap::iterator it = _glyphs.find(c);
		if (it != _glyphs.end()) {
			delete it->second;
			it->second = glyph;
		}
		else
			_glyphs[c] = glyph;
		_changed = true;
	}
}


/** Returns the corresponding glyph data of a given character of the current font.
 *  @param[in] c character code
 *  @return font glyph data (0 if no matching data was found) */
const Glyph* FontCache::getGlyph (int c) const {
	GlyphMap::const_iterator it = _glyphs.find(c);
	return (it != _glyphs.end()) ? it->second : 0;
}


/** Writes the current cache data to a file (only if anything changed after
 *  the last call of read()).
 *  @param[in] fontname name of current font
 *  @param[in] dir directory where the cache file should go
 *  @return true if writing was successful */
bool FontCache::write (const char *fontname, const char *dir) const {
	if (!_changed)
		return true;

	if (fontname && strlen(fontname) > 0) {
		if (dir == 0 || strlen(dir) == 0)
			dir = FileSystem::getcwd().c_str();
		ostringstream oss;
		oss << dir << '/' << fontname << ".fgd";
//		ogzstream ofs(oss.str().c_str(), 9, ios::binary|ios::out);
		ofstream ofs(oss.str().c_str(), ios::binary);
		return write(fontname, ofs);
	}
	return false;
}


/** Returns the minimal number of bytes needed to store the given value. */
static int max_int_size (Int32 value) {
	Int32 limit = 0x7f;
	for (int i=1; i <= 4; i++) {
		if ((value < 0  && -value <= limit+1) || (value >= 0 && value <= limit))
			return i;
		limit = (limit << 8) | 0xff;
	}
	return 4;
}


/** Returns the minimal number of bytes needed to store the biggest
 *  pair component of the given vector. */
static int max_int_size (const vector<LPair> &pairs) {
	int ret=0;
	FORALL(pairs, vector<LPair>::const_iterator, it) {
		ret = max(ret, max_int_size(it->x()));
		ret = max(ret, max_int_size(it->y()));
	}
	return ret;
}


/** Writes the current cache data to a stream (only if anything changed after
 *  the last call of read()).
 *  @param[in] fontname name of current font
 *  @param[in] os output stream
 *  @return true if writing was successful */
bool FontCache::write (const char *fontname, ostream &os) const {
	if (!_changed)
		return true;

	if (os) {
		write_unsigned(VERSION, 1, os);
		for (const char *p=fontname; *p; p++)
			os.put(*p);
		os.put(0);
		write_unsigned(_glyphs.size(), 4, os);
		FORALL(_glyphs, GlyphMap::const_iterator, it) {
			write_unsigned(it->first, 4, os);
			write_unsigned(it->second->commands().size(), 2, os);
			FORALL(it->second->commands(), list<GlyphCommand*>::const_iterator, cit) {
				const vector<LPair> &params = (*cit)->params();
				int bytes = max_int_size(params);
				UInt8 cmdchar = (bytes << 5) | ((*cit)->getSVGPathCommand() - 'A');
				write_unsigned(cmdchar, 1, os);
				for (size_t i=0; i < params.size(); i++) {
					write_signed(params[i].x(), bytes, os);
					write_signed(params[i].y(), bytes, os);
				}
			}
		}
		return true;
	}
	return false;
}


/** Reads font glyph information from a file.
 *  @param[in] fontname name of font data to read
 *  @param[in] dir directory where the cache files are located
 *  @return true if reading was successful */
bool FontCache::read (const char *fontname, const char *dir) {
	clear();
	if (fontname && strlen(fontname) > 0) {
		if (dir == 0 || strlen(dir) == 0)
			dir = FileSystem::getcwd().c_str();
		ostringstream oss;
		oss << dir << '/' << fontname << ".fgd";
		ifstream ifs(oss.str().c_str(), ios::binary);
//		igzstream ifs(oss.str().c_str(), 9, ios::binary|ios::in);
		return read(fontname, ifs);
	}
	return false;
}


/** Reads font glyph information from a stream.
 *  @param[in] fontname name of font data to read
 *  @param[in] dir input stream
 *  @return true if reading was successful */
bool FontCache::read (const char *fontname, istream &is) {
	clear();
	if (is) {
		if (read_unsigned(1, is) != VERSION)
			return false;
		string fname;
		while (!is.eof() && is.peek() != 0)
			fname += is.get();
		is.get(); // skip 0-byte
		if (fname != fontname)
			return false;
		UInt32 num_glyphs = read_unsigned(4, is);
		while (num_glyphs-- > 0) {
			UInt32 c = read_unsigned(4, is);  // character code
			UInt16 s = read_unsigned(2, is);  // number of path commands
			Glyph *glyph = new Glyph;
			while (s-- > 0) {
				UInt8 cmdval = read_unsigned(1, is);
				UInt8 cmdchar = (cmdval & 0x1f) + 'A';
				int bytes = cmdval >> 5;
				GlyphCommand *cmd=0;
				switch (cmdchar) {
					case 'C': {
						LPair p1 = read_pair(bytes, is);
						LPair p2 = read_pair(bytes, is);
						LPair p3 = read_pair(bytes, is);
						cmd = new GlyphCubicTo(p1, p2, p3);
						break;
					}
					case 'H':
						cmd = new GlyphHorizontalLineTo(read_pair(bytes, is));
						break;
					case 'L':
						cmd = new GlyphLineTo(read_pair(bytes, is));
						break;
					case 'M':
						cmd = new GlyphMoveTo(read_pair(bytes, is));
						break;
					case 'Q': {
						LPair p1 = read_pair(bytes, is);
						LPair p2 = read_pair(bytes, is);
						cmd = new GlyphConicTo(p1, p2);
						break;
					}
					case 'S': {
						LPair p1 = read_pair(bytes, is);
						LPair p2 = read_pair(bytes, is);
						cmd = new GlyphShortCubicTo(p1, p2);
						break;
					}
					case 'T':
						cmd = new GlyphShortConicTo(read_pair(bytes, is));
						break;
					case 'V':
						cmd = new GlyphVerticalLineTo(read_pair(bytes, is));
						break;
					case 'Z':
						cmd = new GlyphClosePath();
				}
				if (cmd)
					glyph->addCommand(cmd);
			}
			setGlyph(c, glyph);
		}
		_changed = false;
		return true;
	}
	return false;
}


/** Collects font cache information.
 *  @param[in]  dirname path to font cache directory
 *  @param[out] infos the collected data
 *  @return true on success */
bool FontCache::fontinfo (const char *dirname, std::vector<FontInfo> &infos) {
	infos.clear();
	if (dirname) {
		vector<string> fnames;
		FileSystem::collect(dirname, fnames);
		FORALL(fnames, vector<string>::iterator, it) {
			if ((*it)[0] == 'f' && it->length() > 5 && it->substr(it->length()-4) == ".fgd") {
				FontInfo info;
				string path = string(dirname)+"/"+(it->substr(1));
				ifstream ifs(path.c_str(), ios::binary);
				if (fontinfo(ifs, info))
					infos.push_back(info);
			}
		}
	}
	return infos.size() > 0;
}


/** Collects font cache information of a single font.
 *  @param[in]  is input stream of the cache file
 *  @param[out] info the collected data
 *  @return true on success */
bool FontCache::fontinfo (std::istream &is, FontInfo &info) {
	info.name.clear();
	info.numchars = info.numbytes = info.numcmds = 0;
	if (is) {
		if ((info.version = read_unsigned(1, is)) != VERSION)
			return false;
		while (!is.eof() && is.peek() != 0)
			info.name += is.get();
		is.get(); // skip 0-byte
		info.numchars = read_unsigned(4, is);
		for (UInt32 i=0; i < info.numchars; i++) {
			read_unsigned(4, is);  // character code
			UInt16 s = read_unsigned(2, is);  // number of path commands
			while (s-- > 0) {
				UInt8 cmdval = read_unsigned(1, is);
				UInt8 cmdchar = (cmdval & 0x1f) + 'A';
				int bytes = cmdval >> 5;
				int bc = 0;
				switch (cmdchar) {
					case 'C': bc = 6*bytes; break;
					case 'H':
					case 'L':
					case 'M':
					case 'T':
					case 'V': bc = 2*bytes; break;
					case 'Q':
					case 'S': bc = 4*bytes; break;
					case 'Z': break;
					default : return false;
				}
				info.numbytes += bc+1; // command length + command
				info.numcmds++;
				is.seekg(bc, ios_base::cur);
			}
			info.numbytes += 6; // number of path commands + char code
		}
		info.numbytes += 6+info.name.length(); // version + 0-byte + fontname + number of chars
	}
	return true;
}


/** Collects font cache information and write it to a stream.
 *  @param[in] dirname path to font cache directory
 *  @param[in] os output is written to this stream */
void FontCache::fontinfo (const char *dirname, ostream &os) {
	vector<FontInfo> infos;
	if (fontinfo(dirname, infos)) {
		os << "cache format version " << infos[0].version << endl;
		typedef map<string,FontInfo*> SortMap;
		SortMap sortmap;
		FORALL(infos, vector<FontInfo>::iterator, it)
			sortmap[it->name] = &(*it);

		FORALL(sortmap, SortMap::iterator, it) {
			os << setw(10) << left  << it->second->name
				<< setw(5)  << right << it->second->numchars << " chars"
				<< setw(10) << right << it->second->numcmds  << " cmds"
				<< setw(10) << right << it->second->numbytes << " bytes"
				<< endl;
		}
	}
	else
		os << "cache is empty\n";
}
