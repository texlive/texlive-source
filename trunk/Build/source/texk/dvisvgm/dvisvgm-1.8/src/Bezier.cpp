/*************************************************************************
** Bezier.cpp                                                           **
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

#include <utility>
#include "Bezier.h"

using namespace std;

Bezier::Bezier () {
	_points[0] = _points[1] = _points[2] = _points[3] = 0;
}


/** Creates a quadratic Bézier curve. internally, it's represented as a cubic one. */
Bezier::Bezier (const DPair &p0, const DPair &p1, const DPair &p2) {
	_points[0] = p0;
	_points[1] = p0+(p1-p0)*2.0/3.0;
	_points[2] = p2+(p1-p2)*2.0/3.0;
	_points[3] = p2;
}


Bezier::Bezier (const DPair &p0, const DPair &p1, const DPair &p2, const DPair &p3) {
	_points[0] = p0;
	_points[1] = p1;
	_points[2] = p2;
	_points[3] = p3;
}


/** Creates a subcurve of a given Bézier curve.
 *  @param[in] source original curve to be clipped
 *  @param[in] t0 'time' parameter \f$\in[0,1]\f$ of source curve where the subcurve starts
 *  @param[in] t1 'time' parameter \f$\in[0,1]\f$ of source curve where the subcurve ends */
Bezier::Bezier (const Bezier &source, double t0, double t1) {
	if (t0 == t1)
		_points[0] = _points[1] = _points[2] = _points[3] = source.pointAt(t0);
	else {
		if (t0 > t1)
			swap(t0, t1);
		if (t0 == 0)
			source.subdivide(t1, this, 0);
		else if (t1 == 1)
			source.subdivide(t0, 0, this);
		else {
			Bezier subcurve;
			source.subdivide(t0, 0, &subcurve);
			subcurve.subdivide((t1-t0)/(1-t0), this, 0);
		}
	}
}


void Bezier::reverse() {
	swap(_points[0], _points[3]);
	swap(_points[1], _points[2]);
}


DPair Bezier::pointAt (double t) const {
	const double s = 1-t;
	return _points[0]*s*s*s + _points[1]*3.0*s*s*t + _points[2]*3.0*s*t*t + _points[3]*t*t*t;
}


/** Splits the curve at t into two sub-curves. */
void Bezier::subdivide (double t, Bezier *bezier1, Bezier *bezier2) const {
	const double s = 1-t;
	DPair p01   = _points[0]*s + _points[1]*t;
	DPair p12   = _points[1]*s + _points[2]*t;
	DPair p23   = _points[2]*s + _points[3]*t;
	DPair p012  = p01*s + p12*t;
	DPair p123  = p12*s + p23*t;
	DPair p0123 = p012*s + p123*t;
	if (bezier1) {
		bezier1->_points[0] = _points[0];
		bezier1->_points[1] = p01;
		bezier1->_points[2] = p012;
		bezier1->_points[3] = p0123;
	}
	if (bezier2) {
		bezier2->_points[0] = p0123;
		bezier2->_points[1] = p123;
		bezier2->_points[2] = p23;
		bezier2->_points[3] = _points[3];
	}
}


/** Approximates the current Bézier curve by a sequence of line segments.
 *  This is done by subdividing the curve several times using De Casteljau's algorithm.
 *  If a sub-curve is almost flat, i.e. \f$\sum\limits_{k=0}^2 |p_{k+1}-p_k| - |p_3-p_0| < \delta\f$,
 *  the curve is not further subdivided.
 *  @param[in] delta threshold where to stop further subdivisions (see description above)
 *  @param[out] p the resulting sequence of points defining the start/end points of the line segments
 *  @param[out] t corresponding curve parameters of the approximated points p: \f$ b(t_i)=p_i \f$
 *  @return number of points in vector p */
int Bezier::approximate (double delta, std::vector<DPair> &p, vector<double> *t) const {
	p.push_back(_points[0]);
	if (t)
		t->push_back(0);
	return approximate(delta, 0, 1, p, t);
}


int Bezier::approximate (double delta, double t0, double t1, vector<DPair> &p, vector<double> *t) const {
	// compute distance of adjacent control points
	const double l01 = (_points[1]-_points[0]).length();
	const double l12 = (_points[2]-_points[1]).length();
	const double l23 = (_points[3]-_points[2]).length();
	const double l03 = (_points[3]-_points[0]).length();
	if (l01+l12+l23-l03 < delta) { // is curve flat enough?
		p.push_back(_points[3]);    // => store endpoint
		if (t)
			t->push_back(t1);
	}
	else {
		// subdivide curve at b(0.5) and approximate the resulting parts separately
		Bezier b1, b2;
		subdivide(0.5, &b1, &b2);
		double tmid = (t0+t1)/2;
		b1.approximate(delta, t0, tmid, p, t);
		b2.approximate(delta, tmid, t1, p, t);
	}
	return p.size();
}


static inline double signed_area (const DPair &p1, const DPair &p2, const DPair &p3) {
	return (p2.x()-p1.x())*(p3.y()-p1.y()) - (p3.x()-p1.x())*(p2.y()-p1.y());
}


static inline double dot_prod (const DPair &p1, const DPair &p2) {
	return p1.x()*p2.x() + p1.y()*p2.y();
}


static bool between (const DPair &p1, const DPair &p2, const DPair &p3, double delta) {
	if (fabs(signed_area(p1, p2, p3)) < delta) {
		double dotp = dot_prod(p2-p1, p3-p1);
		return dotp > 0 && dotp < dot_prod(p2-p1, p2-p1);
	}
	return false;
}


static inline bool near (const DPair &p1, const DPair &p2, double delta) {
	DPair diff = p2-p1;
	return fabs(diff.x()) < delta && fabs(diff.y()) < delta;
}


int Bezier::reduceDegree (double delta, vector<DPair> &p) const {
	if (near(_points[0], _points[1], delta) && near(_points[0], _points[2], delta) && near(_points[0], _points[3], delta))
		p.push_back(_points[0]);
	else if (between(_points[0], _points[1], _points[3], delta) && between(_points[0], _points[2], _points[3], delta)) {
		p.push_back(_points[1]);
		p.push_back(_points[3]);
	}
	else if (near((_points[1]-_points[0])*1.5+_points[0], (_points[2]-_points[3])*1.5+_points[3], delta)) {
		p.push_back(_points[0]);
		p.push_back((_points[1]-_points[0])*1.5 + _points[0]);
		p.push_back(_points[3]);
	}
	else {
		for (int i=0; i < 4; i++)
			p.push_back(p[i]);
	}
	return p.size()-1;
}
