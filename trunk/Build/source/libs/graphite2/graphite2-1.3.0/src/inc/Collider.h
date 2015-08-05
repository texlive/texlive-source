/*  GRAPHITE2 LICENSING

    Copyright 2010, SIL International
    All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should also have received a copy of the GNU Lesser General Public
    License along with this library in the file named "LICENSE".
    If not, write to the Free Software Foundation, 51 Franklin Street, 
    Suite 500, Boston, MA 02110-1335, USA or visit their web page on the 
    internet at http://www.fsf.org/licenses/lgpl.html.

Alternatively, the contents of this file may be used under the terms of the
Mozilla Public License (http://mozilla.org/MPL) or the GNU General Public
License, as published by the Free Software Foundation, either version 2
of the License or (at your option) any later version.
*/
#pragma once

#include <utility>
#include "inc/List.h"
#include "inc/Slot.h"
#include "inc/Position.h"
#include "inc/Intervals.h"
#include "inc/debug.h"
//#include "inc/Segment.h"

namespace graphite2 {

class json;
class Slot;
class Segment;

#define SLOTCOLSETUINTPROP(x, y) uint16 x() const { return _ ##x; } void y (uint16 v) { _ ##x = v; }
#define SLOTCOLSETINTPROP(x, y) int16 x() const { return _ ##x; } void y (int16 v) { _ ##x = v; }
#define SLOTCOLSETPOSITIONPROP(x, y) const Position &x() const { return _ ##x; } void y (const Position &v) { _ ##x = v; }

// Slot attributes related to collision-fixing
class SlotCollision
{
public:
    enum {
    //  COLL_TESTONLY = 0,  // default - test other glyphs for collision with this one, but don't move this one
        COLL_FIX = 1,       // fix collisions involving this glyph
        COLL_IGNORE = 2,    // ignore this glyph altogether
        COLL_START = 4,     // start of range of possible collisions
        COLL_END = 8,       // end of range of possible collisions
        COLL_KERN = 16,     // collisions with this glyph are fixed by adding kerning space after it
        COLL_ISCOL = 32,    // this glyph has a collision
        COLL_KNOWN = 64,    // we've figured out what's happening with this glyph
        COLL_TEMPLOCK = 128,    // Lock glyphs that have been given priority positioning
        ////COLL_JUMPABLE = 128,    // moving glyphs may jump this stationary glyph in any direction - DELETE
        ////COLL_OVERLAP = 256,    // use maxoverlap to restrict - DELETE
    };
    
    // Behavior for the collision.order attribute. To GDL this is an enum, to us it's a bitfield, with only 1 bit set
    // Allows for easier inversion.
    enum {
        SEQ_ORDER_LEFTDOWN = 1,
        SEQ_ORDER_RIGHTUP = 2,
        SEQ_ORDER_NOABOVE = 4,
        SEQ_ORDER_NOBELOW = 8,
        SEQ_ORDER_NOLEFT = 16,
        SEQ_ORDER_NORIGHT = 32
    };
    
    SlotCollision(Segment *seg, Slot *slot);
    void initFromSlot(Segment *seg, Slot *slot);
    
    const Rect &limit() const { return _limit; }
    void setLimit(const Rect &r) { _limit = r; }
    SLOTCOLSETPOSITIONPROP(shift, setShift)
    SLOTCOLSETPOSITIONPROP(offset, setOffset)
    SLOTCOLSETPOSITIONPROP(exclOffset, setExclOffset)
    SLOTCOLSETUINTPROP(margin, setMargin)
    SLOTCOLSETUINTPROP(marginWt, setMarginWt)
    SLOTCOLSETUINTPROP(flags, setFlags)
    SLOTCOLSETUINTPROP(exclGlyph, setExclGlyph)
    SLOTCOLSETUINTPROP(seqClass, setSeqClass)
    SLOTCOLSETUINTPROP(seqProxClass, setSeqProxClass)
    SLOTCOLSETUINTPROP(seqOrder, setSeqOrder)
    SLOTCOLSETINTPROP(seqAboveXoff, setSeqAboveXoff)
    SLOTCOLSETUINTPROP(seqAboveWt, setSeqAboveWt)
    SLOTCOLSETINTPROP(seqBelowXlim, setSeqBelowXlim)
    SLOTCOLSETUINTPROP(seqBelowWt, setSeqBelowWt)
    SLOTCOLSETUINTPROP(seqValignHt, setSeqValignHt)
    SLOTCOLSETUINTPROP(seqValignWt, setSeqValignWt)

    float getKern(int dir) const;
    
private:
    Rect        _limit;
    Position    _shift;     // adjustment within the given pass
    Position    _offset;    // total adjustment for collisions
    Position    _exclOffset;
    uint16		_margin;
    uint16		_marginWt;
    uint16		_flags;
    uint16		_exclGlyph;
    uint16		_seqClass;
	uint16		_seqProxClass;
    uint16		_seqOrder;
    int16		_seqAboveXoff;
    uint16		_seqAboveWt;
    int16		_seqBelowXlim;
    uint16		_seqBelowWt;
    uint16		_seqValignHt;
    uint16		_seqValignWt;
	
};  // end of class SlotColllision

class BBox;
class SlantBox;

class ShiftCollider
{
public:
    typedef std::pair<float, float> fpair;
    typedef Vector<fpair> vfpairs;
    typedef vfpairs::iterator ivfpairs;

    ShiftCollider(GR_MAYBE_UNUSED json *dbgout)
    {
#if !defined GRAPHITE2_NTRACING
        for (int i = 0; i < 4; ++i)
            _ranges[i].setdebug(dbgout);
#endif
    }
    ~ShiftCollider() throw() { };

    bool initSlot(Segment *seg, Slot *aSlot, const Rect &constraint,
                float margin, float marginMin, const Position &currShift,
                const Position &currOffset, int dir, GR_MAYBE_UNUSED json * const dbgout);
    bool mergeSlot(Segment *seg, Slot *slot, const Position &currShift, bool isAfter, 
                bool sameCluster, bool &hasCol, bool isExclusion, GR_MAYBE_UNUSED json * const dbgout);
    Position resolve(Segment *seg, bool &isCol, GR_MAYBE_UNUSED json * const dbgout);
    void addBox_slope(bool isx, const Rect &box, const BBox &bb, const SlantBox &sb, const Position &org, float weight, float m, bool minright, int mode);
    void removeBox(const Rect &box, const BBox &bb, const SlantBox &sb, const Position &org, int mode);
    const Position &origin() const { return _origin; }

#if !defined GRAPHITE2_NTRACING
	void outputJsonDbg(json * const dbgout, Segment *seg, int axis);
	void outputJsonDbgStartSlot(json * const dbgout, Segment *seg);
	void outputJsonDbgEndSlot(json * const dbgout, Position resultPos, int bestAxis, bool isCol);
	void outputJsonDbgOneVector(json * const dbgout, Segment *seg, int axis, float tleft, float bestCost, float bestVal);
	void outputJsonDbgRawRanges(json * const dbgout, int axis);
	void outputJsonDbgRemovals(json * const dbgout, int axis, Segment *seg);
#endif

    CLASS_NEW_DELETE;

protected:
    Zones _ranges[4];   // possible movements in 4 directions (horizontally, vertically, diagonally);
    Slot *  _target;    // the glyph to fix
    Rect    _limit;
    Position _currShift;
    Position _currOffset;
    Position _origin;   // Base for all relative calculations
    float   _margin;
	float	_marginWt;
    float   _len[4];
    uint16  _seqClass;
	uint16	_seqProxClass;
    uint16  _seqOrder;
    
	//bool _scraping[4];

};	// end of class ShiftCollider

class KernCollider
{
public:
    KernCollider(GR_MAYBE_UNUSED json *dbg) : _miny(-1e38f), _maxy(1e38f) { };
    ~KernCollider() throw() { };
    bool initSlot(Segment *seg, Slot *aSlot, const Rect &constraint, float margin,
            const Position &currShift, const Position &offsetPrev, int dir,
            float ymin, float ymax, json * const dbgout);
    bool mergeSlot(Segment *seg, Slot *slot, const Position &currShift, float currSpace, int dir, json * const dbgout);
    Position resolve(Segment *seg, Slot *slot, int dir, float margin, json * const dbgout);
    void shift(const Position &mv, int dir);

    CLASS_NEW_DELETE;

private:
    Slot *  _target;        // the glyph to fix
    Rect    _limit;
    float   _margin;
    Position _offsetPrev;   // kern from a previous pass
    Position _currShift;    // NOT USED??
    float _miny;	        // y-coordinates offset by global slot position
    float _maxy;
    Vector<float> _edges;   // edges of horizontal slices
    float _sliceWidth;      // width of each slice
    float _mingap;
    float _xbound;        // max or min edge

#if !defined GRAPHITE2_NTRACING    
    // Debugging
    Segment * _seg;
    Vector<float> _nearEdges; // closest potential collision in each slice
    Vector<Slot*> _slotNear;
#endif
};	// end of class KernCollider


inline
float sqr(float x) { return x * x; }


};  // end of namespace graphite2

