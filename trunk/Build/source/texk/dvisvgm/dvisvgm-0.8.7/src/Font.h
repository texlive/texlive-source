/*************************************************************************
** Font.h                                                               **
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

#ifndef FONT_H
#define FONT_H

#include <map>
#include <string>
#include <vector>
#include "MessageException.h"
#include "VFActions.h"
#include "VFReader.h"
#include "types.h"

using std::map;
using std::string;
using std::vector;


class FontEncoding;
class TFM;


/** Abstract base for all font classes. */
struct Font
{
	virtual ~Font () {}
	virtual Font* clone (double ds, double sc) const =0;
	virtual const Font* uniqueFont () const =0;
	virtual string name () const =0;
	virtual double designSize () const =0;
	virtual double scaledSize () const =0;
	virtual double scaleFactor () const        {return scaledSize()/designSize();}
	virtual double charWidth (int c) const =0;
	virtual double charDepth (int c) const =0;
	virtual double charHeight (int c) const =0;
	virtual double italicCorr (int c) const =0;
	virtual const TFM* getTFM () const =0;
	virtual const char* path () const =0;
};


/** Empty font without any glyphs. Instances of this class are used
 *  if no physical or virtual font file can be found.
 *  The metric values returned by the member functions are based on cmr10. */
struct EmptyFont : public Font
{
	public:
		EmptyFont (string name) : fontname(name) {}
		Font* clone (double ds, double sc) const {return new EmptyFont(*this);}
		const Font* uniqueFont () const {return this;}
		string name () const            {return fontname;}
		double designSize () const      {return 10;}    // cmr10 design size in pt
		double scaledSize () const      {return 10;}    // cmr10 scaled size in pt
		double charWidth (int c) const  {return 9.164;} // width of cmr10's 'M' in pt
		double charHeight (int c) const {return 6.833;} // height of cmr10's 'M' in pt
		double charDepth (int c) const  {return 0;}
		double italicCorr (int c) const {return 0;}
		const TFM* getTFM () const      {return 0;}
		const char* path () const       {return 0;}

	private:
		string fontname;
};


/** Interface for all physical fonts. */
struct PhysicalFont : public virtual Font
{
	enum Type {MF, PFB, TTF};
	static Font* create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type);
	virtual Type type () const =0;
};


/** Interface for all virtual fonts. */
class VirtualFont : public virtual Font
{
	friend class FontManager;
	public:
		typedef vector<UInt8> DVIVector;

	public:
		static Font* create (string name, UInt32 checksum, double dsize, double ssize);
		virtual const DVIVector* getDVI (int c) const =0;

	protected:
		virtual void assignChar (UInt32 c, DVIVector *dvi) =0;
};


class TFMFont : public virtual Font
{
	public:
		TFMFont (string name, UInt32 checksum, double dsize, double ssize);
		~TFMFont ();
		const TFM* getTFM () const;
		string name () const        {return fontname;}
		double designSize () const  {return dsize;}
		double scaledSize () const  {return ssize;}
		double charWidth (int c) const;
		double charDepth (int c) const;
		double charHeight (int c) const;
		double italicCorr (int c) const;

	private:
		mutable TFM *tfm;
		string fontname;
		UInt32 checksum; ///< cheksum to be compared with TFM checksum
		double dsize;    ///< design size in TeX point units
		double ssize;    ///< scaled size
};


class PhysicalFontProxy : public PhysicalFont
{
	friend class PhysicalFontImpl;
	public:
		Font* clone (double ds, double sc) const {return new PhysicalFontProxy(*this, ds, sc);}
		const Font* uniqueFont () const       {return pf;}
		string name () const                  {return pf->name();}
		double designSize () const            {return dsize;}
		double scaledSize () const            {return ssize;}
		double charWidth (int c) const        {return pf->charWidth(c);}
		double charDepth (int c) const        {return pf->charDepth(c);}
		double charHeight (int c) const       {return pf->charHeight(c);}
		double italicCorr (int c) const       {return pf->italicCorr(c);}
		const TFM* getTFM () const            {return pf->getTFM();}
		const char* path () const             {return pf->path();}
		Type type () const                    {return pf->type();}

	protected:
		PhysicalFontProxy (const PhysicalFont *font, double ds, double ss) : pf(font), dsize(ds), ssize(ss) {}
		PhysicalFontProxy (const PhysicalFontProxy &proxy, double ds, double ss) : pf(proxy.pf), dsize(ds), ssize(ss) {}

	private:
		const PhysicalFont *pf;
		double dsize;  ///< design size in TeX point units
		double ssize;  ///< scaled size
};


class PhysicalFontImpl : public PhysicalFont, public TFMFont
{
	friend class PhysicalFont;
	public:
		Font* clone (double ds, double ss) const {return new PhysicalFontProxy(this, ds, ss);}
		const Font* uniqueFont () const {return this;}
		const char* path () const;
		Type type () const {return filetype;}

	protected:
		PhysicalFontImpl (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type);

	private:
		Type filetype;
};


class VirtualFontProxy : public VirtualFont
{
	friend class VirtualFontImpl;
	public:
		Font* clone (double ds, double ss) const {return new VirtualFontProxy(*this, ds, ss);}
		const Font* uniqueFont () const       {return vf;}
		string name () const                  {return vf->name();}
		const DVIVector* getDVI (int c) const {return vf->getDVI(c);}
		double designSize () const            {return dsize;}
		double scaledSize () const            {return ssize;}
		double charWidth (int c) const        {return vf->charWidth(c);}
		double charDepth (int c) const        {return vf->charDepth(c);}
		double charHeight (int c) const       {return vf->charHeight(c);}
		double italicCorr (int c) const       {return vf->italicCorr(c);}
		const TFM* getTFM () const            {return vf->getTFM();}
		const char* path () const             {return vf->path();}

	protected:
		VirtualFontProxy (const VirtualFont *font, double ds, double ss) : vf(font), dsize(ds), ssize(ss) {}
		VirtualFontProxy (const VirtualFontProxy &proxy, double ds, double ss) : vf(proxy.vf), dsize(ds), ssize(ss) {}
		void assignChar (UInt32 c, DVIVector *dvi) {delete dvi;}

	private:
		const VirtualFont *vf;
		double dsize;  ///< design size in TeX point units
		double ssize;  ///< scaled size in TeX point units
};


class VirtualFontImpl : public VirtualFont, public TFMFont
{
	friend class VirtualFont;
	public:
		~VirtualFontImpl ();
		Font* clone (double ds, double ss) const {return new VirtualFontProxy(this, ds, ss);}
		const Font* uniqueFont () const   {return this;}
		const DVIVector* getDVI (int c) const;
		const char* path () const;

	protected:
		VirtualFontImpl (string name, UInt32 checksum, double dsize, double ssize);
		void assignChar (UInt32 c, DVIVector *dvi);

	private:
		map<UInt32, DVIVector*> charDefs; ///< dvi subroutines defining the characters
};


struct FontException : public MessageException
{
	FontException (string msg) : MessageException(msg) {}
};

#endif
