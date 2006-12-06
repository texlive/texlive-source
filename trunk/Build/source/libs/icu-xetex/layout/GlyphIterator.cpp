/*
 *
 * (C) Copyright IBM Corp. 1998-2005 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "OpenTypeTables.h"
#include "GlyphDefinitionTables.h"
#include "GlyphPositionAdjustments.h"
#include "GlyphIterator.h"
#include "LEGlyphStorage.h"
#include "Lookups.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

GlyphIterator::GlyphIterator(LEGlyphStorage &theGlyphStorage, GlyphPositionAdjustments *theGlyphPositionAdjustments, le_bool rightToLeft, le_uint16 theLookupFlags, LETag theFeatureTag,
    const GlyphDefinitionTableHeader *theGlyphDefinitionTableHeader)
  : direction(1), position(-1), nextLimit(-1), prevLimit(-1),
    glyphStorage(theGlyphStorage), glyphPositionAdjustments(theGlyphPositionAdjustments),
    srcIndex(-1), destIndex(-1), lookupFlags(theLookupFlags), featureTag(theFeatureTag), featureParam(0),
    glyphClassDefinitionTable(NULL), markAttachClassDefinitionTable(NULL)

{
    le_int32 glyphCount = glyphStorage.getGlyphCount();

    if (theGlyphDefinitionTableHeader != NULL) {
        glyphClassDefinitionTable = theGlyphDefinitionTableHeader->getGlyphClassDefinitionTable();
        markAttachClassDefinitionTable = theGlyphDefinitionTableHeader->getMarkAttachClassDefinitionTable();
    }

    nextLimit = glyphCount;

    if (rightToLeft) {
        direction = -1;
        position = glyphCount;
        nextLimit = -1;
        prevLimit = glyphCount;
    }
}

GlyphIterator::GlyphIterator(GlyphIterator &that)
  : glyphStorage(that.glyphStorage)
{
    direction    = that.direction;
    position     = that.position;
    nextLimit    = that.nextLimit;
    prevLimit    = that.prevLimit;

    glyphPositionAdjustments = that.glyphPositionAdjustments;
    srcIndex = that.srcIndex;
    destIndex = that.destIndex;
    lookupFlags = that.lookupFlags;
    featureTag = that.featureTag;
    featureParam = that.featureParam;
    glyphClassDefinitionTable = that.glyphClassDefinitionTable;
    markAttachClassDefinitionTable = that.markAttachClassDefinitionTable;
}

GlyphIterator::GlyphIterator(GlyphIterator &that, LETag newFeatureTag, le_int32 newFeatureParam)
  : glyphStorage(that.glyphStorage)
{
    direction    = that.direction;
    position     = that.position;
    nextLimit    = that.nextLimit;
    prevLimit    = that.prevLimit;

    glyphPositionAdjustments = that.glyphPositionAdjustments;
    srcIndex = that.srcIndex;
    destIndex = that.destIndex;
    lookupFlags = that.lookupFlags;
    featureTag = newFeatureTag;
    featureParam = newFeatureParam;
    glyphClassDefinitionTable = that.glyphClassDefinitionTable;
    markAttachClassDefinitionTable = that.markAttachClassDefinitionTable;
}

GlyphIterator::GlyphIterator(GlyphIterator &that, le_uint16 newLookupFlags)
  : glyphStorage(that.glyphStorage)
{
    direction    = that.direction;
    position     = that.position;
    nextLimit    = that.nextLimit;
    prevLimit    = that.prevLimit;

    glyphPositionAdjustments = that.glyphPositionAdjustments;
    srcIndex = that.srcIndex;
    destIndex = that.destIndex;
    lookupFlags = newLookupFlags;
    featureTag = that.featureTag;
    featureParam = that.featureParam;
    glyphClassDefinitionTable = that.glyphClassDefinitionTable;
    markAttachClassDefinitionTable = that.markAttachClassDefinitionTable;
}

GlyphIterator::~GlyphIterator()
{
    // nothing to do, right?
}

void GlyphIterator::reset(le_uint16 newLookupFlags, LETag newFeatureTag)
{
    position    = prevLimit;
    featureTag  = newFeatureTag;
    featureParam = 0;
    lookupFlags = newLookupFlags;
}

LEGlyphID *GlyphIterator::insertGlyphs(le_int32 count)
{
    return glyphStorage.insertGlyphs(position, count);
}

le_int32 GlyphIterator::applyInsertions()
{
    le_int32 newGlyphCount = glyphStorage.applyInsertions();

    if (direction < 0) {
        prevLimit = newGlyphCount;
    } else {
        nextLimit = newGlyphCount;
    }

    return newGlyphCount;
}

le_int32 GlyphIterator::getCurrStreamPosition() const
{
    return position;
}

le_int32 GlyphIterator::getFeatureParam() const
{
	return featureParam;
}

le_bool GlyphIterator::isRightToLeft() const
{
    return direction < 0;
}

le_bool GlyphIterator::ignoresMarks() const
{
    return (lookupFlags & lfIgnoreMarks) != 0;
}

le_bool GlyphIterator::baselineIsLogicalEnd() const
{
    return (lookupFlags & lfBaselineIsLogicalEnd) != 0;
}

LEGlyphID GlyphIterator::getCurrGlyphID() const
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return 0xFFFF;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return 0xFFFF;
        }
    }

    return glyphStorage[position];
}

void GlyphIterator::getCursiveEntryPoint(LEPoint &entryPoint) const
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->getEntryPoint(position, entryPoint);
}

void GlyphIterator::getCursiveExitPoint(LEPoint &exitPoint) const
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->getExitPoint(position, exitPoint);
}

void GlyphIterator::setCurrGlyphID(TTGlyphID glyphID)
{
    LEGlyphID glyph = glyphStorage[position];

    glyphStorage[position] = LE_SET_GLYPH(glyph, glyphID);
}

void GlyphIterator::setCurrStreamPosition(le_int32 newPosition)
{
    if (direction < 0) {
        if (newPosition >= prevLimit) {
            position = prevLimit;
            return;
        }

        if (newPosition <= nextLimit) {
            position = nextLimit;
            return;
        }
    } else {
        if (newPosition <= prevLimit) {
            position = prevLimit;
            return;
        }

        if (newPosition >= nextLimit) {
            position = nextLimit;
            return;
        }
    }

    position = newPosition - direction;
    next();
}

void GlyphIterator::setCurrGlyphBaseOffset(le_int32 baseOffset)
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->setBaseOffset(position, baseOffset);
}

void GlyphIterator::adjustCurrGlyphPositionAdjustment(float xPlacementAdjust, float yPlacementAdjust,
                                                      float xAdvanceAdjust, float yAdvanceAdjust)
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->adjustXPlacement(position, xPlacementAdjust);
    glyphPositionAdjustments->adjustYPlacement(position, yPlacementAdjust);
    glyphPositionAdjustments->adjustXAdvance(position, xAdvanceAdjust);
    glyphPositionAdjustments->adjustYAdvance(position, yAdvanceAdjust);
}

void GlyphIterator::setCurrGlyphPositionAdjustment(float xPlacementAdjust, float yPlacementAdjust,
                                                      float xAdvanceAdjust, float yAdvanceAdjust)
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->setXPlacement(position, xPlacementAdjust);
    glyphPositionAdjustments->setYPlacement(position, yPlacementAdjust);
    glyphPositionAdjustments->setXAdvance(position, xAdvanceAdjust);
    glyphPositionAdjustments->setYAdvance(position, yAdvanceAdjust);
}

void GlyphIterator::setCursiveEntryPoint(LEPoint &entryPoint)
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->setEntryPoint(position, entryPoint, baselineIsLogicalEnd());
}

void GlyphIterator::setCursiveExitPoint(LEPoint &exitPoint)
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->setExitPoint(position, exitPoint, baselineIsLogicalEnd());
}

void GlyphIterator::setCursiveGlyph()
{
    if (direction < 0) {
        if (position <= nextLimit || position >= prevLimit) {
            return;
        }
    } else {
        if (position <= prevLimit || position >= nextLimit) {
            return;
        }
    }

    glyphPositionAdjustments->setCursiveGlyph(position, baselineIsLogicalEnd());
}

le_bool GlyphIterator::filterGlyph(le_uint32 index) const
{
    LEGlyphID glyphID = glyphStorage[index];
    le_int32 glyphClass = gcdNoGlyphClass;

    if (LE_GET_GLYPH(glyphID) >= 0xFFFE) {
        return TRUE;
    }

    if (glyphClassDefinitionTable != NULL) {
        glyphClass = glyphClassDefinitionTable->getGlyphClass(glyphID);
    }

    switch (glyphClass)
    {
    case gcdNoGlyphClass:
        return FALSE;

    case gcdSimpleGlyph:
        return (lookupFlags & lfIgnoreBaseGlyphs) != 0;

    case gcdLigatureGlyph:
        return (lookupFlags & lfIgnoreLigatures) != 0;

    case gcdMarkGlyph:
    {
        if ((lookupFlags & lfIgnoreMarks) != 0) {
            return TRUE;
        }

        le_uint16 markAttachType = (lookupFlags & lfMarkAttachTypeMask) >> lfMarkAttachTypeShift;

        if ((markAttachType != 0) && (markAttachClassDefinitionTable != NULL)) {
            return markAttachClassDefinitionTable->getGlyphClass(glyphID) != markAttachType;
        }

        return FALSE;
    }

    case gcdComponentGlyph:
        return (lookupFlags & lfIgnoreBaseGlyphs) != 0;

    default:
        return FALSE;
    }
}

static const LETag emptyTag = 0;
static const LETag defaultTag = 0xFFFFFFFF;

le_bool GlyphIterator::hasFeatureTag()
{
    if (featureTag == defaultTag || featureTag == emptyTag) {
    	featureParam = 0;
        return TRUE;
    }

    LEErrorCode success = LE_NO_ERROR;
    const LETag *tagList = (const LETag *) glyphStorage.getAuxData(position, success);

    if (tagList != NULL) {
        for (le_int32 tag = 0; tagList[tag] != emptyTag; tag += 1) {
            if (tagList[tag] == featureTag) {
				const le_int32 *paramList = (const le_int32 *) glyphStorage.getAuxData2(position, success);
				if (paramList != NULL)
					featureParam = paramList[tag];
				else
					featureParam = 0;
                return TRUE;
            }
        }
    }

    return FALSE;
}

le_bool GlyphIterator::findFeatureTag()
{
    while (nextInternal()) {
        if (hasFeatureTag()) {
            prevInternal();
            return TRUE;
        }
    }

    return FALSE;
}


le_bool GlyphIterator::nextInternal(le_uint32 delta)
{
    le_int32 newPosition = position;

    while (newPosition != nextLimit && delta > 0) {
        do {
            newPosition += direction;
        } while (newPosition != nextLimit && filterGlyph(newPosition));

        delta -= 1;
    }

    position = newPosition;

    return position != nextLimit;
}

le_bool GlyphIterator::next(le_uint32 delta)
{
    return nextInternal(delta) && hasFeatureTag();
}

le_bool GlyphIterator::prevInternal(le_uint32 delta)
{
    le_int32 newPosition = position;

    while (newPosition != prevLimit && delta > 0) {
        do {
            newPosition -= direction;
        } while (newPosition != prevLimit && filterGlyph(newPosition));

        delta -= 1;
    }

    position = newPosition;

    return position != prevLimit;
}

le_bool GlyphIterator::prev(le_uint32 delta)
{
    return prevInternal(delta) && hasFeatureTag();
}

le_int32 GlyphIterator::getMarkComponent(le_int32 markPosition) const
{
    le_int32 component = 0;
    le_int32 posn;

    for (posn = position; posn != markPosition; posn += direction) {
        if (glyphStorage[posn] == 0xFFFE) {
            component += 1;
        }
    }

    return component;
}

// This is basically prevInternal except that it
// doesn't take a delta argument, and it doesn't
// filter out 0xFFFE glyphs.
le_bool GlyphIterator::findMark2Glyph()
{
    le_int32 newPosition = position;

    do {
        newPosition -= direction;
    } while (newPosition != prevLimit && glyphStorage[newPosition] != 0xFFFE && filterGlyph(newPosition));

    position = newPosition;

    return position != prevLimit;
}

U_NAMESPACE_END
