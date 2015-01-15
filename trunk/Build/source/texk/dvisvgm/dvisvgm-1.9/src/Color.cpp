/*************************************************************************
** Color.cpp                                                            **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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

#define _USE_MATH_DEFINES
#include <config.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "Color.h"

using namespace std;


const Color Color::BLACK(UInt32(0));
const Color Color::WHITE(UInt8(255), UInt8(255), UInt8(255));
const Color Color::TRANSPARENT(UInt32(0xff000000));


static inline UInt8 double_to_byte (double v) {
	v = max(0.0, min(1.0, v));
	return UInt8(floor(255*v+0.5));
}


static void tolower (string &str) {
	for (size_t i=0; i < str.length(); i++)
		str[i] = tolower(str[i]);
}


Color::Color (const char *name) {
	if (!setName(name, false))
		setGray(UInt8(0));
}


Color::Color (const string &name) {
	if (!setName(name, false))
		setGray(UInt8(0));
}


void Color::setRGB (double r, double g, double b) {
	setRGB(double_to_byte(r), double_to_byte(g), double_to_byte(b));
}


bool Color::setName (string name, bool case_sensitive) {
	if (name[0] == '#') {
		char *p=0;
		_rgb = UInt32(strtol(name.c_str()+1, &p, 16));
		while (isspace(*p))
			p++;
		return (*p == 0 && _rgb <= 0xFFFFFF);
	}
	// converted color constants from color.pro
	static const struct ColorConstant {
		const char *name;
		const UInt32 rgb;
	}
	constants[] = {
		{"Apricot",        0xFFAD7A},
		{"Aquamarine",     0x2DFFB2},
		{"Bittersweet",    0xC10200},
		{"Black",          0x000000},
		{"Blue",           0x0000FF},
		{"BlueGreen",      0x26FFAA},
		{"BlueViolet",     0x190CF4},
		{"BrickRed",       0xB70000},
		{"Brown",          0x660000},
		{"BurntOrange",    0xFF7C00},
		{"CadetBlue",      0x606DC4},
		{"CarnationPink",  0xFF5EFF},
		{"Cerulean",       0x0FE2FF},
		{"CornflowerBlue", 0x59DDFF},
		{"Cyan",           0x00FFFF},
		{"Dandelion",      0xFFB528},
		{"DarkOrchid",     0x9932CC},
		{"Emerald",        0x00FF7F},
		{"ForestGreen",    0x00E000},
		{"Fuchsia",        0x7202EA},
		{"Goldenrod",      0xFFE528},
		{"Gray",           0x7F7F7F},
		{"Green",          0x00FF00},
		{"GreenYellow",    0xD8FF4F},
		{"JungleGreen",    0x02FF7A},
		{"Lavender",       0xFF84FF},
		{"LimeGreen",      0x7FFF00},
		{"Magenta",        0xFF00FF},
		{"Mahogany",       0xA50000},
		{"Maroon",         0xAD0000},
		{"Melon",          0xFF897F},
		{"MidnightBlue",   0x007091},
		{"Mulberry",       0xA314F9},
		{"NavyBlue",       0x0F75FF},
		{"OliveGreen",     0x009900},
		{"Orange",         0xFF6321},
		{"OrangeRed",      0xFF007F},
		{"Orchid",         0xAD5BFF},
		{"Peach",          0xFF7F4C},
		{"Periwinkle",     0x6D72FF},
		{"PineGreen",      0x00BF28},
		{"Plum",           0x7F00FF},
		{"ProcessBlue",    0x0AFFFF},
		{"Purple",         0x8C23FF},
		{"RawSienna",      0x8C0000},
		{"Red",            0xFF0000},
		{"RedOrange",      0xFF3A21},
		{"RedViolet",      0x9600A8},
		{"Rhodamine",      0xFF2DFF},
		{"RoyalBlue",      0x007FFF},
		{"RoyalPurple",    0x3F19FF},
		{"RubineRed",      0xFF00DD},
		{"Salmon",         0xFF779E},
		{"SeaGreen",       0x4FFF7F},
		{"Sepia",          0x4C0000},
		{"SkyBlue",        0x60FFE0},
		{"SpringGreen",    0xBCFF3D},
		{"Tan",            0xDB9370},
		{"TealBlue",       0x1EF9A3},
		{"Thistle",        0xE068FF},
		{"Turquoise",      0x26FFCC},
		{"Violet",         0x351EFF},
		{"VioletRed",      0xFF30FF},
		{"White",          0xFFFFFF},
		{"WildStrawberry", 0xFF0A9B},
		{"Yellow",         0xFFFF00},
		{"YellowGreen",    0x8EFF42},
		{"YellowOrange",   0xFF9300},
	};
	if (!case_sensitive) {
		tolower(name);
		for (size_t i=0; i < sizeof(constants)/sizeof(ColorConstant); i++) {
			string cmpname = constants[i].name;
			tolower(cmpname);
			if (name == cmpname) {
				_rgb = constants[i].rgb;
				return true;
			}
		}
		return false;
	}

	// binary search
	int first=0, last=sizeof(constants)/sizeof(ColorConstant)-1;
	while (first <= last) {
		int mid = first+(last-first)/2;
		int cmp = strcmp(constants[mid].name, name.c_str());
		if (cmp > 0)
			last = mid-1;
		else if (cmp < 0)
			first = mid+1;
		else {
			_rgb = constants[mid].rgb;
			return true;
		}
	}
	return false;
}


void Color::setHSB (double h, double s, double b) {
	valarray<double> hsb(3), rgb(3);
	hsb[0] = h;
	hsb[1] = s;
	hsb[2] = b;
	HSB2RGB(hsb, rgb);
	setRGB(rgb);
}


void Color::setCMYK (double c, double m, double y, double k) {
	valarray<double> cmyk(4), rgb(3);
	cmyk[0] = c;
	cmyk[1] = m;
	cmyk[2] = y;
	cmyk[3] = k;
	CMYK2RGB(cmyk, rgb);
	setRGB(rgb);
}


void Color::setCMYK (const std::valarray<double> &cmyk) {
	valarray<double> rgb(3);
	CMYK2RGB(cmyk, rgb);
	setRGB(rgb);
}


void Color::set (ColorSpace colorSpace, VectorIterator<double> &it) {
	switch (colorSpace) {
		case GRAY_SPACE: setGray(*it++); break;
		case RGB_SPACE : setRGB(*it, *(it+1), *(it+2)); it+=3; break;
		case LAB_SPACE : setLab(*it, *(it+1), *(it+2)); it+=3; break;
		case CMYK_SPACE: setCMYK(*it, *(it+1), *(it+2), *(it+3)); it+=4; break;
	}
}


void Color::operator *= (double c) {
	UInt32 rgb=0;
	for (int i=0; i < 3; i++) {
		rgb |= UInt32(floor((_rgb & 0xff)*c+0.5)) << (8*i);
		_rgb >>= 8;
	}
	_rgb = rgb;
}


string Color::rgbString () const {
	ostringstream oss;
	oss << '#';
	for (int i=2; i >= 0; i--) {
		oss << setbase(16) << setfill('0') << setw(2)
			 << (((_rgb >> (8*i)) & 0xff));
	}
	return oss.str();
}


/** Approximates a CMYK color by an RGB triple. The component values
 *  are expected to be normalized, i.e. 0 <= cmyk[i],rgb[j] <= 1.
 *  @param[in]  cmyk color in CMYK space
 *  @param[out] rgb  RGB approximation */
void Color::CMYK2RGB (const valarray<double> &cmyk, valarray<double> &rgb) {
	double kc = 1.0-cmyk[3];
	for (int i=0; i < 3; i++)
		rgb[i] = min(1.0, max(0.0, (1.0-cmyk[i])*kc));
}


void Color::RGB2CMYK (const valarray<double> &rgb, valarray<double> &cmyk) {
	double c = 1-rgb[0];
	double m = 1-rgb[1];
	double y = 1-rgb[2];
	cmyk[3] = min(min(c, m), y);
	cmyk[0] = c-cmyk[3];
	cmyk[1] = m-cmyk[3];
	cmyk[2] = y-cmyk[3];
}


/** Converts a color given in HSB coordinates to RGB.
 *  @param[in]  hsb color in HSB space
 *  @param[out] rgb color in RGB space */
void Color::HSB2RGB (const valarray<double> &hsb, valarray<double> &rgb) {
	if (hsb[1] == 0)
		rgb[0] = rgb[1] = rgb[2] = hsb[2];
	else {
		double h = hsb[0]-floor(hsb[0]);
		int i = int(6*h);
		double f = 6*h-i;
		double p = hsb[2]*(1-hsb[1]);
		double q = hsb[2]*(1-hsb[1]*f);
		double t = hsb[2]*(1-hsb[1]*(1-f));
		switch (i) {
			case 0 : rgb[0]=hsb[2]; rgb[1]=t; rgb[2]=p; break;
			case 1 : rgb[0]=q; rgb[1]=hsb[2]; rgb[2]=p; break;
			case 2 : rgb[0]=p; rgb[1]=hsb[2]; rgb[2]=t; break;
			case 3 : rgb[0]=p; rgb[1]=q; rgb[2]=hsb[2]; break;
			case 4 : rgb[0]=t; rgb[1]=p; rgb[2]=hsb[2]; break;
			case 5 : rgb[0]=hsb[2]; rgb[1]=p; rgb[2]=q; break;
			default: ;  // prevent compiler warning
		}
	}
}


double Color::getGray () const {
	double r, g, b;
	getRGB(r, g, b);
	return r*0.3 + g*0.59 + b*0.11; // gray value according to NTSC video standard
}


void Color::getGray (valarray<double> &gray) const {
	gray.resize(1);
	gray[0] = getGray();
}


void Color::getRGB (double &r, double &g, double &b) const {
	r = ((_rgb >> 16) & 255) / 255.0;
	g = ((_rgb >> 8) & 255) / 255.0;
	b = (_rgb & 255) / 255.0;
}


void Color::getRGB (valarray<double> &rgb) const {
	rgb.resize(3);
	double r, g, b;
	getRGB(r, g, b);
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


void Color::getCMYK (double &c, double &m, double &y, double &k) const {
	valarray<double> rgb(3), cmyk(4);
	getRGB(rgb);
	RGB2CMYK(rgb, cmyk);
	c = cmyk[0];
	m = cmyk[1];
	y = cmyk[2];
	k = cmyk[3];
}


void Color::getCMYK (std::valarray<double> &cmyk) const {
	cmyk.resize(4);
	valarray<double> rgb(3);
	getRGB(rgb);
	RGB2CMYK(rgb, cmyk);
}


void Color::getXYZ (double &x, double &y, double &z) const {
	valarray<double> rgb(3), xyz(3);
	getRGB(rgb);
	RGB2XYZ(rgb, xyz);
	x = xyz[0];
	y = xyz[1];
	z = xyz[2];
}


void Color::setXYZ (double x, double y, double z) {
	valarray<double> xyz(3), rgb(3);
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;
	XYZ2RGB(xyz, rgb);
	setRGB(rgb);
}


void Color::setXYZ (const valarray<double> &xyz) {
	valarray<double> rgb(3);
	XYZ2RGB(xyz, rgb);
	setRGB(rgb);
}


void Color::setLab (double l, double a, double b) {
	valarray<double> lab(3), xyz(3);
	lab[0] = l;
	lab[1] = a;
	lab[2] = b;
	Lab2XYZ(lab, xyz);
	setXYZ(xyz);
}


void Color::setLab (const valarray<double> &lab) {
	valarray<double> xyz(3);
	Lab2XYZ(lab, xyz);
	setXYZ(xyz);
}


/** Get the color in CIELAB color space using the sRGB working space and reference white D65. */
void Color::getLab (double &l, double &a, double &b) const {
	valarray<double> rgb(3), lab(3);
	getRGB(rgb);
	RGB2Lab(rgb, lab);
	l = lab[0];
	a = lab[1];
	b = lab[2];
}


void Color::getLab (std::valarray<double> &lab) const {
	lab.resize(3);
	valarray<double> rgb(3);
	getRGB(rgb);
	RGB2Lab(rgb, lab);
}


static inline double sqr (double x)  {return x*x;}
static inline double cube (double x) {return x*x*x;}


void Color::Lab2XYZ (const valarray<double> &lab, valarray<double> &xyz) {
	xyz.resize(3);
	double wx=0.95047, wy=1.00, wz=1.08883;  // reference white D65
	double eps = 0.008856;
	double kappa = 903.3;
	double fy = (lab[0]+16)/116;
	double fx = lab[1]/500 + fy;
	double fz = fy - lab[2]/200;
	double xr = (cube(fx) > eps ? cube(fx) : (116*fx-16)/kappa);
	double yr = (lab[0] > kappa*eps ? cube((lab[0]+16)/116) : lab[0]/kappa);
	double zr = (cube(fz) > eps ? cube(fz) : (116*fz-16)/kappa);
	xyz[0] = xr*wx;
	xyz[1] = yr*wy;
	xyz[2] = zr*wz;
}


void Color::XYZ2RGB (const valarray<double> &xyz, valarray<double> &rgb) {
	rgb.resize(3);
	rgb[0] =  3.2404542*xyz[0] - 1.5371385*xyz[1] - 0.4985314*xyz[2];
	rgb[1] = -0.9692660*xyz[0] + 1.8760108*xyz[1] + 0.0415560*xyz[2];
	rgb[2] =  0.0556434*xyz[0] - 0.2040259*xyz[1] + 1.0572252*xyz[2];
	for (int i=0; i < 3; i++)
		rgb[i] = (rgb[i] <= 0.0031308 ? 12.92*rgb[i] : 1.055*pow(rgb[i], 1/2.4)-0.055);
}


void Color::RGB2XYZ (valarray<double> rgb, valarray<double> &xyz) {
	xyz.resize(3);
	for (int i=0; i < 3; i++)
		rgb[i] = (rgb[i] <= 0.04045 ? rgb[i]/12.92 : pow((rgb[i]+0.055)/1.055, 2.4));
	xyz[0] = 0.4124564*rgb[0] + 0.3575761*rgb[1] + 0.1804375*rgb[2];
	xyz[1] = 0.2126729*rgb[0] + 0.7151522*rgb[1] + 0.0721750*rgb[2];
	xyz[2] = 0.0193339*rgb[0] + 0.1191920*rgb[1] + 0.9503041*rgb[2];
}


void Color::RGB2Lab (const valarray<double> &rgb, valarray<double> &lab) {
	double wx=0.95047, wy=1.00, wz=1.08883;  // reference white D65
	double eps = 0.008856;
	double kappa = 903.3;
	valarray<double> xyz(3);
	RGB2XYZ(rgb, xyz);
	xyz[0] /= wx;
	xyz[1] /= wy;
	xyz[2] /= wz;
	double fx = (xyz[0] > eps ? pow(xyz[0], 1.0/3) : (kappa*xyz[0]+16)/116);
	double fy = (xyz[1] > eps ? pow(xyz[1], 1.0/3) : (kappa*xyz[1]+16)/116);
	double fz = (xyz[2] > eps ? pow(xyz[2], 1.0/3) : (kappa*xyz[2]+16)/116);
	lab[0] = 116*fy-16;
	lab[1] = 500*(fx-fy);
	lab[2] = 200*(fy-fz);
}


/** Returns the Delta E difference (CIE 2000) between this and another color. */
double Color::deltaE (const Color &c) const {
	double l1, a1, b1;
	double l2, a2, b2;
	getLab(l1, a1, b1);
	c.getLab(l2, a2, b2);
	double dl = l2-l1;
	double lm = (l1+l2)/2;
	double c1 = sqrt(a1*a1 + b1*b1);
	double c2 = sqrt(a2*a2 + b2*b2);
	double cm = (c1+c2)/2;
	double g  = (1-sqrt(pow(cm, 7)/(pow(cm, 7)+pow(25.0, 7))))/2;
	double aa1 = a1*(1+g);
	double aa2 = a2*(1+g);
	double cc1 = sqrt(aa1*aa1 + b1*b1);
	double cc2 = sqrt(aa2*aa2 + b2*b2);
	double ccm = (cc1+cc2)/2;
	double dcc = cc2-cc1;
	double h1  = atan2(b1, aa1)*180/M_PI;
	if (h1 < 0) h1 += 360;
	double h2  = atan2(b2, aa2)*180/M_PI;
	if (h2 < 0)	h2 += 360;
	double hm = (abs(h1-h2) > 180 ? (h1+h2+360) : (h1+h2))/2;
	double t  = 1 - 0.17*cos(hm-30) + 0.24*cos(2*hm) + 0.32*cos(3*hm+6) - 0.2*cos(4*hm-63);
	double dh = h2-h1;
	if (h2-h1 < -180)
		dh += 360;
	else if (h2-h1 > 180)
		dh -= 360;
	double dhh = 2*sqrt(cc1*cc2)*sin(dh/2);
	double sl = 1 + 0.015*(lm-50.0)*(lm-50.0)/sqrt(20.0+(lm-50.0));
	double sc = 1 + 0.045*ccm;
	double sh = 1 + 0.015*ccm*t;
	double dtheta = 30*exp(-sqr(hm-275)/25);
	double rc = 2*sqrt(pow(ccm, 7)/(pow(ccm, 7)+pow(25.0, 7)));
	double rt = -rc*sin(2*dtheta);
	return sqrt(sqr(dl/sl) + sqr(dcc/sc) + sqr(dhh/sh) + rt*dcc/sc*dhh/sh);
}


int Color::numComponents (ColorSpace colorSpace) {
	switch (colorSpace) {
		case GRAY_SPACE: return 1;
		case LAB_SPACE:
		case RGB_SPACE:  return 3;
		case CMYK_SPACE: return 4;
	}
	return 0;
}