/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2008 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TransductionLog.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Contains the functions for writing a log of the transduction process using an XML format.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <math.h>

#undef THIS_FILE
DEFINE_THIS_FILE

//:End Ignore

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local Constants and static variables
//:>********************************************************************************************

namespace gr
{

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************

#define SP_PER_SLOT 7
#define LEADING_SP 15
#define MAX_SLOTS 128

/*----------------------------------------------------------------------------------------------
	Output a file showing a log of the transduction process and the resulting segment.
----------------------------------------------------------------------------------------------*/
bool GrTableManager::WriteXmlLog(std::ostream * pstrmLog,
	GrCharStream * pchstrm, Segment * psegRet, int cbPrevSegDat, byte * pbPrevSegDat)
{
#ifdef TRACING
	if (!pstrmLog)
		return false;
	std::ostream & strmOut = *pstrmLog;
	WriteXmlLogAux(strmOut, pchstrm, psegRet, cbPrevSegDat, pbPrevSegDat);
	return true;
#else
	return false;
#endif // TRACING
}


bool GrTableManager::WriteXmlAssocLog(std::ostream * pstrmLog,
	GrCharStream * pchstrm, Segment * psegRet)
{
#ifdef TRACING
	if (!pstrmLog)
		return false;

	std::ostream & strmOut = *pstrmLog;
	LogFinalPositions(strmOut);

	LogXmlTagOpen(strmOut, "Mappings", 1, true);
	LogXmlTagPostAttrs(strmOut, true);

	//psegRet->LogXmlUnderlyingToSurface(strmOut, this, pchstrm, 2);
	//psegRet->LogXmlSurfaceToUnderlying(strmOut, this, 2);

	LogXmlTagClose(strmOut, "Mappings", 1, true);
	
	strmOut << "\n";
	LogXmlTagClose(strmOut, "GraphiteTraceLog", 0, true);

	return true;
#else
	return false;
#endif // TRACING
}

#ifdef TRACING

/*----------------------------------------------------------------------------------------------
	Output a file showing a log of the transduction process and the resulting segment.
----------------------------------------------------------------------------------------------*/
void GrTableManager::WriteXmlLogAux(std::ostream & strmOut,
	GrCharStream * pchstrm, Segment * psegRet,
	int cbPrevSegDat, byte * pbPrevSegDat)
{
	strmOut
		<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		<< "<!DOCTYPE GraphiteTraceLog SYSTEM \"GraphiteTraceLog.dtd\">\n"
		<< "\n";

	LogXmlTagOpen(strmOut, "GraphiteTraceLog", 0, true);
	LogXmlTagPostAttrs(strmOut, true);
	strmOut << "\n";

	LogXmlTagOpen(strmOut, "SegmentRun", 1, true);
	LogXmlTagAttr(strmOut, "stringOffset", pchstrm->Min(), 0);
	LogXmlTagPostAttrs(strmOut, true);

	if (cbPrevSegDat == 0)
		LogXmlUnderlying(strmOut, pchstrm, 0, 2);
	else
	{
		Assert(*(pbPrevSegDat + 4) == 0);	// skip offset for first pass
		LogXmlUnderlying(strmOut, pchstrm, *(pbPrevSegDat + 3), 2);
	}

	//LogXmlPass1Input(strmOut);

	for (int ipass = 0; ipass < m_cpass; ipass++)
	{
		if (cbPrevSegDat == 0)
			LogXmlPass(strmOut, ipass, 0, 2);
		else
			LogXmlPass(strmOut, ipass, *(pbPrevSegDat + 4 + ipass), 2);
	}

	LogXmlTagClose(strmOut, "SegmentRun", 1, true);
	strmOut << "\n";

	// Don't do this until after we've logged the associations.
	//LogXmlTagClose(strmOut, "GraphiteTraceLog", 0, true);
}

/*----------------------------------------------------------------------------------------------
	Write out a log of the underlying input.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlUnderlying(std::ostream & strmOut, GrCharStream * pchstrm,
	int cchwBackup, size_t nIndent)
{
	LogXmlTagOpen(strmOut, "Input", nIndent, true);
	LogXmlTagPostAttrs(strmOut, true);

	LogXmlUnderlyingAux(strmOut, pchstrm, cchwBackup, -1, nIndent,
		true,	// text
		true,	// features
		true,	// color
		false,	// string offset
		false,	// ligature components
		false);	// glyph associations

	LogXmlTagClose(strmOut, "Input", nIndent, true);
}

void GrTableManager::LogXmlUnderlyingAux(std::ostream & strmOut, GrCharStream * pchstrm,
	int cch32Backup, int cch32Lim, size_t nIndent,
	bool fLogText, bool fLogFeatures, bool fLogColor, bool fLogStrOff, bool fLogLig, bool fLogGlyphs)
{
	int rgnChars[MAX_SLOTS];
	bool rgfNewRun[MAX_SLOTS];
	std::fill_n(rgfNewRun, MAX_SLOTS, false);
	GrFeatureValues rgfval[MAX_SLOTS];
	int cchwMaxRawChars;

	int cch32 = pchstrm->GetLogData(this, rgnChars, rgfNewRun, rgfval,
		cch32Backup, &cchwMaxRawChars);
	cch32 = min(cch32, MAX_SLOTS);
	if (cch32Lim > -1)
		cch32 = min(cch32Lim, cch32);

	int ichw;

	// For UTF-8 and surrogate representation:
	int   rgchwChars1[MAX_SLOTS];
	utf16 rgchwChars2[MAX_SLOTS];
	utf16 rgchwChars3[MAX_SLOTS];
	utf16 rgchwChars4[MAX_SLOTS];
	utf16 rgchwChars5[MAX_SLOTS];
	utf16 rgchwChars6[MAX_SLOTS];
	utf16 rgiRawString[MAX_SLOTS];	// indices from underlying slots to text-source chars (0-based)

	int rgichwRaw[MAX_SLOTS]; // index of input char with in the given UTF32, 1-based

	if (cchwMaxRawChars > 1)
	{
		cchwMaxRawChars = min(cchwMaxRawChars, 6); // max of 6 raw (UTF-8 or UTF-16) chars per slot
		pchstrm->GetLogDataRaw(this, cch32, cch32Backup, cchwMaxRawChars,
			rgchwChars1, rgchwChars2, rgchwChars3, rgchwChars4, rgchwChars5, rgchwChars6,
			rgichwRaw);
	}
	else
	{
		for (ichw = 0; ichw < cch32; ichw++)
		{
			rgichwRaw[ichw] = 1;
			rgchwChars1[ichw] = rgnChars[ichw];
			rgchwChars2[ichw] = 0;
			rgchwChars3[ichw] = 0;
			rgchwChars4[ichw] = 0;
			rgchwChars5[ichw] = 0;
			rgchwChars6[ichw] = 0;
		}
	}

	int ichUtf32 = 0;
	ichw = 0;
	while (ichUtf32 < cch32)
	{
		if (rgichwRaw[ichw] == 1)
		{
			rgiRawString[ichUtf32++] = (utf16)ichw;
		}
		ichw++;
	}

	int ichwFeat = 0;
	for (ichw = 0; ichw < cch32; ichw++)
	{
		if (rgfNewRun[ichw])
			ichwFeat = ichw;	// because rgfval only saves features for first of run

		// See if there are any features.
		bool fFeatures = false;
		size_t ifeat;
		for (ifeat = 0; fLogFeatures && ifeat < kMaxFeatures; ifeat++)
		{
			if (rgfval[ichwFeat].FeatureValue(ifeat) != 0)
			{
				fFeatures = true;
				break;
			}
		}

		bool fContent = fFeatures;

		LogXmlTagOpen(strmOut, "Char", nIndent+1, fContent);

		LogXmlTagAttr(strmOut, "index", rgiRawString[ichw] - cch32Backup, 0);
		LogXmlTagAttrHex(strmOut, "usv", rgnChars[ichw], 0);

		if (fLogText)
		{
			char rgch[20];
			rgch[0] = (char)rgnChars[ichw];
			rgch[1] = 0;
			if (rgnChars[ichw] < 0x0100 && rgchwChars2[ichw] == 0)	// ANSI
				LogXmlTagAttr(strmOut, "text", rgch);
			else if (rgnChars[ichw] == knLRM)
				LogXmlTagAttr(strmOut, "text", "<LRM>");
			else if (rgnChars[ichw] == knRLM)
				LogXmlTagAttr(strmOut, "text", "<RLM>");
			else if (rgnChars[ichw] == knLRO)
				LogXmlTagAttr(strmOut, "text", "<LRO>");
			else if (rgnChars[ichw] == knRLO)
				LogXmlTagAttr(strmOut, "text", "<RLO>");
			else if (rgnChars[ichw] == knLRE)
				LogXmlTagAttr(strmOut, "text", "<LRE>");
			else if (rgnChars[ichw] == knRLE)
				LogXmlTagAttr(strmOut, "text", "<RLE>");
			else if (rgnChars[ichw] == knPDF)
				LogXmlTagAttr(strmOut, "text", "<PDF>");
		}

		if (fLogStrOff)
			LogXmlTagAttr(strmOut, "stringOffset", pchstrm->Min() + rgiRawString[ichw] - cch32Backup, 0);

		if (rgchwChars2[ichw] != 0)
		{
			std::string strHex = HexString(rgchwChars1[ichw], rgchwChars2[ichw], rgchwChars3[ichw],
				rgchwChars4[ichw], rgchwChars5[ichw], rgchwChars6[ichw]);
			if (pchstrm->TextSrc()->utfEncodingForm() == kutf8)
                LogXmlTagAttr(strmOut, "textSourceUtf8", strHex.c_str());
			else
				LogXmlTagAttr(strmOut, "textSourceUtf16", strHex.c_str());
		}

		if (fLogColor)
		{
			IColorTextSource * tscolor = dynamic_cast<IColorTextSource *>(pchstrm->TextSrc());
			if (tscolor)
			{
				int clrFore, clrBack;
				tscolor->getColors(ichw, &clrFore, &clrBack);
				LogXmlTagColor(strmOut, "color", clrFore, false);
				LogXmlTagColor(strmOut, "background", clrBack, true);
			}
		}

		if (fLogLig)
		{

		}

//if (fLogLig || fLogGlyphs)
//{
//	size_t cassocs = 0;
//	int fLigs = false;
//	int ichw;
//	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
//	{
//		if (m_prgpvisloutAssocs[ichw])
//			cassocs = max(cassocs, m_prgpvisloutAssocs[ichw]->size());
//		if (m_prgisloutLigature[ichw] != kNegInfinity)
//			fLigs = true;
//	}
//}

		LogXmlTagPostAttrs(strmOut, fContent);

		if (fLogFeatures)
		{
			for (ifeat = 0; ifeat < kMaxFeatures; ifeat++)
			{
				if (rgfval[ichwFeat].FeatureValue(ifeat) != 0)
				{
					GrFeature * pfeat = Feature(ifeat);

					LogXmlTagOpen(strmOut, "Feature", nIndent+2, false);
					LogXmlTagAttr(strmOut, "id", pfeat->ID());
					LogXmlTagAttr(strmOut, "value", rgfval[ichwFeat].FeatureValue(ifeat));
					//LogXmlTagPostAttrs(strmOut, false);
					LogXmlTagClose(strmOut, "Feature", nIndent+1, false);
				}
			}
		}

		if (fLogGlyphs)
		{
		}

		LogXmlTagClose(strmOut, "Char", nIndent+1, fContent);
	}
}

/*----------------------------------------------------------------------------------------------
	Output the the results of pass.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlPass(std::ostream & strmOut, int ipass, int cslotSkipped, int nIndent)
{
	strmOut << "\n";

	GrPass * ppass = Pass(ipass);
	GrSlotStream * psstrmIn = (ipass == 0) ? NULL : InputStream(ipass);
	GrSlotStream * psstrmOut = OutputStream(ipass);

	int islot;
	//	Mark each slot with its index in the input and output streams.
	for (islot = 0; ipass > 0 && islot < psstrmIn->ReadPos(); islot++)
		psstrmIn->SlotAt(islot)->m_islotTmpIn = islot;

	for (islot = 0; islot < psstrmOut->WritePos(); islot++)
		psstrmOut->SlotAt(islot)->m_islotTmpOut = islot;

	LogXmlTagOpen(strmOut, "Pass", nIndent, true);
	LogXmlTagAttr(strmOut, "number", ipass);

	std::string strType;
	if (dynamic_cast<GrGlyphGenPass *>(ppass))
		strType = "glyphGeneration";
	else if (dynamic_cast<GrBidiPass *>(ppass))
		strType = "bidi";
	else if (dynamic_cast<GrSubPass *>(ppass))
	{
		if (ipass >= m_ipassJust1)
			strType = "justification";
		else
			strType = "substitution";
	}
	else if (dynamic_cast<GrPosPass *>(ppass))
		strType = "positioning";
	else if (dynamic_cast<GrLineBreakPass *>(ppass))
		strType = "linebreak";
	LogXmlTagAttr(strmOut, "type", strType.c_str());

	LogXmlTagPostAttrs(strmOut, true);

	if (!dynamic_cast<GrBidiPass *>(ppass) && !dynamic_cast<GrGlyphGenPass *>(ppass))
	{
		LogXmlTagOpen(strmOut, "RulesMatched", nIndent+1, true);
		LogXmlTagPostAttrs(strmOut, true);
		ppass->LogXmlRules(strmOut, this, psstrmIn,  nIndent+2);
		LogXmlTagClose(strmOut, "RulesMatched", nIndent+1, true);
	}

	bool fJustWidths = false;
	if (ipass == m_ipassJust1 - 1 && ShouldLogJustification()
		&& m_engst.m_jmodi == kjmodiJustify)
	{
		fJustWidths = true;
	}

	LogXmlTagOpen(strmOut, "Output", nIndent+1, true);
	LogXmlTagPostAttrs(strmOut, true);
	bool fPreJust = (!fJustWidths && ipass == m_ipassJust1 - 1 && ShouldLogJustification());
	bool fPostJust = (fJustWidths || ipass == m_ipassJust1 && ShouldLogJustification());
	ppass->LogXmlGlyphs(strmOut, this, psstrmOut, m_ipassJust1, fPreJust, fPostJust, cslotSkipped, nIndent+2);
	LogXmlTagClose(strmOut, "Output", nIndent+1, true);

	LogXmlTagClose(strmOut, "Pass", nIndent, true);

#ifdef XMLOMIT

	//	If this was the pass just before the justification routines get run, output a
	//	special line that just shows the results, ie, the values of justify.width.
	//	TODO: adjust this when measure mode get functioning.
	if (ipass == m_ipassJust1 - 1 && ShouldLogJustification()
		&& m_engst.m_jmodi == kjmodiJustify)
	{
		strmOut << "\nJUSTIFICATION\n\n";
		LogSlotHeader(strmOut, psstrmOut->WritePos(), SP_PER_SLOT, LEADING_SP);
		LogSlotGlyphs(strmOut, psstrmOut);
		LogAttributes(strmOut, ipass, true);
	}

#endif // XMLOMIT

}

/*----------------------------------------------------------------------------------------------
	Write the list of rules fired and failed for a given pass.
----------------------------------------------------------------------------------------------*/
void GrPass::LogXmlRules(std::ostream & strmOut, GrTableManager * ptman,
	GrSlotStream * psstrmIn, int nIndent)
{
	m_pzpst->LogXmlRules(strmOut, ptman, psstrmIn, nIndent);
}

void PassState::LogXmlRules(std::ostream & strmOut, GrTableManager * ptman,
	GrSlotStream * psstrmIn, int nIndent)
{
	if (m_crulrec == 0)
	{
		ptman->LogXmlComment(strmOut, "none", nIndent);
		return;
	}

	for (int irulrec = 0; irulrec < m_crulrec; irulrec++)
	{
		bool fSourceAvailable = false;

		ptman->LogXmlTagOpen(strmOut, "Rule", nIndent, fSourceAvailable);
		
		ptman->LogXmlTagAttr(strmOut, "slot", m_rgrulrec[irulrec].m_islot);

		if (m_rgrulrec[irulrec].m_irul == PassState::kHitMaxRuleLoop)
			ptman->LogXmlTagAttr(strmOut, "hitMaxRuleLoop", "true");
		else if (m_rgrulrec[irulrec].m_irul == PassState::kHitMaxBackup)
			ptman->LogXmlTagAttr(strmOut, "hitMaxBackup", "true");
		else
		{
			ptman->LogXmlTagAttr(strmOut, "debugNumber", m_rgrulrec[irulrec].m_irul);
			ptman->LogXmlTagAttr(strmOut, "fired", (m_rgrulrec[irulrec].m_fFired) ? "true" : "false");
		}

		ptman->LogXmlTagPostAttrs(strmOut, fSourceAvailable);

		// TODO: output Source

		ptman->LogXmlTagClose(strmOut, "Rule", nIndent, fSourceAvailable);
	}
}

void GrPass::LogXmlGlyphs(std::ostream & strmOut, GrTableManager * ptman, GrSlotStream * psstrmOut, 
	int ipassJust1, bool fPreJust, bool fPostJust, int cslotSkipped, int nIndent)
{
	GrBidiPass * ppassBidi = dynamic_cast<GrBidiPass *>(this);
	GrBidiPass * ppassBidiNext = (m_ipass + 1 >= ptman->NumberOfPasses()) ?
		NULL :
		dynamic_cast<GrBidiPass *>(ptman->Pass(m_ipass + 1));
	m_pzpst->LogXmlGlyphs(strmOut, ptman, psstrmOut, ipassJust1, fPreJust, fPostJust,
		(ppassBidi != NULL), (ppassBidiNext != NULL), cslotSkipped, nIndent);
}

void PassState::LogXmlGlyphs(std::ostream & strmOut, GrTableManager * ptman, GrSlotStream * psstrmOut,
	int ipassJust1, bool fPreJust, bool fPostJust, bool fBidi, bool fBidiNext, int cslotSkipped,
	int nIndent)
{
	ptman->LogXmlTagOpen(strmOut, "Output", nIndent, true);
	ptman->LogXmlTagPostAttrs(strmOut, true);

	for (int islot = 0; islot < psstrmOut->WritePos() + 1; islot++) // +1 allows us to handle final deletion
	{
		if (islot > MAX_SLOTS)
			break;

		for (int islotDel = 0; islotDel < m_rgcslotDeletions[islot]; islotDel++)
		{
			ptman->LogXmlTagOpen(strmOut, "Glyph", nIndent+1, false);
			ptman->LogXmlTagAttr(strmOut, "slot", "deleted");
			ptman->LogXmlTagPostAttrs(strmOut, false);
			ptman->LogXmlTagClose(strmOut, "Glyph", nIndent+1, false);
		}
		if (islot >= psstrmOut->WritePos())
			break;

		GrSlotState * pslot = psstrmOut->SlotAt(islot);

		bool fMod = (pslot->PassModified() >= m_ipass && m_ipass > 0);
		if (fBidi || fBidiNext)
			fMod = true;

		ptman->LogXmlTagOpen(strmOut, "Glyph", nIndent+1, fMod);

		ptman->LogXmlTagAttr(strmOut, "slot", islot);
		if (pslot->GlyphID() == ptman->LBGlyphID())
			ptman->LogXmlTagAttr(strmOut, "gid", "linebreak");
		else
			ptman->LogXmlTagAttr(strmOut, "gid", pslot->GlyphID());
		if (pslot->GlyphID() != pslot->ActualGlyphForOutput(ptman))
			ptman->LogXmlTagAttr(strmOut, "actual", pslot->ActualGlyphForOutput(ptman));
		if (m_rgfInsertion[islot])
			ptman->LogXmlTagAttr(strmOut, "inserted", "true");
		if (islot < cslotSkipped)
			ptman->LogXmlTagAttr(strmOut, "nextPassSkips", "true");

		ptman->LogXmlTagPostAttrs(strmOut, fMod);

		if (fMod)
		{
			pslot->LogXmlAttributes(strmOut, ptman, psstrmOut, m_ipass, islot,
				fPreJust, fPostJust, fBidi, fBidiNext, nIndent+2);
		}

		ptman->LogXmlTagClose(strmOut, "Glyph", nIndent+1, fMod);
	}

	ptman->LogXmlTagClose(strmOut, "Output", nIndent, true);
}

/*----------------------------------------------------------------------------------------------
	Write out the attributes that have changes.
----------------------------------------------------------------------------------------------*/
void GrSlotState::LogXmlAttributes(std::ostream & strmOut, GrTableManager * ptman,
	GrSlotStream * psstrmOut, int ipass,
	int islot, bool fPreJust, bool fPostJust, bool fBidi, bool fBidiNext, int nIndent)
{
	//	To handle reprocessing, in which case there may be a chain of slots modified
	//	in the same pass:
	GrSlotState * pslotPrev = PrevState();
	while (pslotPrev && pslotPrev->PassModified() == PassModified())
		pslotPrev = pslotPrev->m_pslotPrevState;

	bool * prgfMods = new bool[kslatMax + ptman->NumUserDefn() - 1];
	memset(prgfMods, 0, (kslatMax + ptman->NumUserDefn() - 1) * sizeof(bool));
	int ccomp;
	int cassoc;
	SlotAttrsModified(prgfMods, fPreJust, &ccomp, &cassoc); // just for this one slot

	if (fPreJust)
		prgfMods[kslatJWidth] = false;
	else if (fPostJust)
		prgfMods[kslatJWidth] = true;

	//if (fJustWidths)
	//{}

	bool fLogAssocs = false;

	for (int slat = 0; slat < kslatMax + ptman->NumUserDefn() - 1; slat++)
	{
		if (!prgfMods[slat] && (slat != kslatDir || (!fBidi && !fBidiNext)))
			continue;

		if (slat == kslatPosX)
			continue;
		if (slat == kslatPosY)
			continue;

		std::string strAttrName;
		switch (slat)
		{
		case kslatAdvX:			strAttrName = "advance.x";		break;
		case kslatAdvY:			strAttrName = "advance.y";		break;
		case kslatAttTo:		strAttrName = "att.to";			break;
		case kslatAttAtX:		strAttrName = "att.at.x";		break;
		case kslatAttAtY:		strAttrName = "att.at.y";		break;
		case kslatAttAtGpt:		strAttrName = "att.at.gpt";		break;
		case kslatAttAtXoff:	strAttrName = "att.at.xoff";	break;
		case kslatAttAtYoff:	strAttrName = "att.at.yoff";	break;
		case kslatAttWithX:		strAttrName = "att.with.x";		break;
		case kslatAttWithY:		strAttrName = "att.with.y";		break;
		case kslatAttWithGpt:	strAttrName = "att.with.gpt";	break;
		case kslatAttWithXoff:	strAttrName = "att.with.xoff";	break;
		case kslatAttWithYoff:	strAttrName = "att.with.yoff";	break;
		case kslatAttLevel:		strAttrName = "att.level";		break;
		case kslatBreak:		strAttrName = "breakweight";	break;
		case kslatCompRef:		strAttrName = "component";		break; // << iIndex + 1 - 1-based
		case kslatDir:			strAttrName = "dir";			break;
		case kslatInsert:		strAttrName = "insert";			break;
		case kslatMeasureSol:	strAttrName = "measure.sol";	break;
		case kslatMeasureEol:	strAttrName = "measure.eol";	break;
		case kslatJStretch:		strAttrName = "just.stretch";	break;
		case kslatJShrink:		strAttrName = "just.shrink";	break;
		case kslatJStep:		strAttrName = "just.step";		break;
		case kslatJWeight:		strAttrName = "just.weight";	break;
		case kslatJWidth:		strAttrName = "just.width";		break;
		case kslatPosX:
		case kslatPosY:
			Assert(false);
			break;
		case kslatShiftX:		strAttrName = "shift.x"; break;
		case kslatShiftY:		strAttrName = "shift.y"; break;
		default:
			if (kslatUserDefn <= slat &&
				slat < kslatUserDefn + ptman->NumUserDefn())
			{
				strAttrName.assign("user");
				char rgch[20];
				itoa(slat - kslatUserDefn + 1, rgch, 10);  // 1-based
				strAttrName.append(rgch);
			}
			else
			{
				// Invalid attribute:
				Warn("bad slot attribute");
				break;
			}
		}

		fLogAssocs = (fLogAssocs || prgfMods[slat]);

		if (slat == kslatCompRef)
		{
			for (int icomp = 0; icomp < ccomp; icomp++)
			{
				ptman->LogXmlTagOpen(strmOut, "Attribute", nIndent, false);
				ptman->LogXmlTagAttr(strmOut, "name", strAttrName.c_str());
				ptman->LogXmlTagAttr(strmOut, "index", icomp+1);
				int nValue = GetSlotAttrValue(strmOut, ptman, ipass, slat, icomp, fPreJust, fPostJust);
				ptman->LogXmlTagAttr(strmOut, "newValue", nValue);
				ptman->LogXmlTagPostAttrs(strmOut, false);
				ptman->LogXmlTagClose(strmOut, "Attribute", nIndent, false);
			}
		}
		else
		{
			std::string strValue = "newValue";
			if ((!prgfMods[slat] && fBidiNext) || fBidi)
				strValue = "value";

			ptman->LogXmlTagOpen(strmOut, "Attribute", nIndent, false);
			ptman->LogXmlTagAttr(strmOut, "name", strAttrName.c_str());
			int nValue = GetSlotAttrValue(strmOut, ptman, ipass, slat, 0, fPreJust, fPostJust);
			switch (slat)
			{
			case kslatDir:		ptman->LogXmlDirCode(strmOut, strValue.c_str(), nValue);break;
			case kslatBreak:	ptman->LogXmlBreakWeight(strmOut, "newValue", nValue);	break;
			default:			ptman->LogXmlTagAttr(strmOut, "newValue", nValue);		break;
			}
			ptman->LogXmlTagPostAttrs(strmOut, false);
			ptman->LogXmlTagClose(strmOut, "Attribute", nIndent, false);
		}
	}

	if (fBidi)
	{
		// Output final directionality level
		ptman->LogXmlTagOpen(strmOut, "Attribute", nIndent, false);
		ptman->LogXmlTagAttr(strmOut, "name", "directionLevel");
		ptman->LogXmlTagAttr(strmOut, "value", DirLevel());
		ptman->LogXmlTagPostAttrs(strmOut, false);
		ptman->LogXmlTagClose(strmOut, "Attribute", nIndent, false);
	}

	if (m_vpslotAssoc.size() > 1)
		fLogAssocs = true;	// multiple associations
	else if (m_vpslotAssoc.size() == 1)
	{
		// association change from previous stream
		GrSlotState * pslotAssoc = AssocSlot(0);
		if (pslotAssoc == NULL || pslotAssoc->m_islotTmpIn != islot)
			fLogAssocs = true;
	}
	if (fLogAssocs)
	{
		std::string strAssocs;
		char rgch[20];
		for (size_t iassoc = 0; iassoc < m_vpslotAssoc.size(); iassoc++)
		{
			GrSlotState * pslotAssoc = AssocSlot(iassoc);
			if (pslotAssoc)
			{
				int n = pslotAssoc->m_islotTmpIn;
				memset(rgch,0,20);
				itoa(n,rgch,10);
				if (iassoc > 0)
					strAssocs.append(" ");
				strAssocs.append(rgch);
			}
		}
		if (strAssocs.length() == 0)
			strAssocs.assign("??");
		ptman->LogXmlTagOpen(strmOut, "Associations", nIndent, false);
		ptman->LogXmlTagAttr(strmOut, "slots", strAssocs.c_str());
		//ptman->LogXmlTagPostAttrs(strmOut, false);
		ptman->LogXmlTagClose(strmOut, "Associations", nIndent, false);
	}

	delete prgfMods;
}

#if XMLOMIT
/*----------------------------------------------------------------------------------------------
	Write out the final positions.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogFinalPositions(std::ostream & strmOut)
{
	GrSlotStream * psstrm = OutputStream(m_cpass - 1);

	strmOut << "x position     ";
	for (int islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		if (pslot->IsLineBreak(LBGlyphID()))
		{
			strmOut << "       ";
			continue;
		}
		LogInTable(strmOut, pslot->XPosition());
	}
	strmOut << "\n";

	strmOut << "y position     ";
	for (int islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		if (pslot->IsLineBreak(LBGlyphID()))
		{
			strmOut << "       ";
			continue;
		}
		LogInTable(strmOut, pslot->YPosition());
	}
	strmOut << "\n";
}

#endif // XMLOMIT

/*----------------------------------------------------------------------------------------------
	Log the value of the slot attribute for the given slot. Assume it has changed.
----------------------------------------------------------------------------------------------*/
int GrSlotState::GetSlotAttrValue(std::ostream & strmOut, GrTableManager * ptman,
	int ipass, int slat, int iIndex, bool fPreJust, bool fPostJust)
{
	switch (slat)
	{
	case kslatAdvX:
		return int(m_mAdvanceX);
	case kslatAdvY:
		return int(m_mAdvanceY);

	case kslatAttTo:
		return m_srAttachTo;

	case kslatAttAtX:		// always do these in pairs
		return m_mAttachAtX;
	case kslatAttAtY:
		return m_mAttachAtY;
	case kslatAttAtGpt:
		return m_nAttachAtGpoint;

	case kslatAttAtXoff:	// always do these in pairs
		return m_mAttachAtXOffset;
	case kslatAttAtYoff:
		return m_mAttachAtYOffset;

	case kslatAttWithX:		// always do these in pairs
		return m_mAttachWithX;
	case kslatAttWithY:
		return m_mAttachWithY;

	case kslatAttWithGpt:
		return m_nAttachWithGpoint;

	case kslatAttWithXoff:	// always do these in pairs
		return m_mAttachWithXOffset;
	case kslatAttWithYoff:
		return m_mAttachWithYOffset;

	case kslatAttLevel:
		return m_nAttachLevel;

	case kslatBreak:
		return m_lb;

	case kslatDir:
		return m_dirc;

	case kslatInsert:
		return m_fInsertBefore;

	case kslatShiftX:
		return m_mShiftX;
	case kslatShiftY:
		return m_mShiftY;
	case kslatMeasureSol:
		return m_mMeasureSol;
	case kslatMeasureEol:
		return m_mMeasureEol;
	case kslatJStretch:
		return m_mJStretch0;
	case kslatJShrink:
		return m_mJShrink0;
	case kslatJStep:
		return m_mJStep0;
	case kslatJWeight:
		return m_nJWeight0;
	case kslatJWidth:
		return m_mJWidth0;

	case kslatCompRef:
		{
		GrSlotState * pslotComp = reinterpret_cast<GrSlotState *>(CompRef(iIndex));
		return pslotComp->m_islotTmpIn;
		}

	default:
		if (kslatUserDefn <= slat && slat <= kslatUserDefn + m_cnUserDefn)
		{
			int iTmp = slat - kslatUserDefn;
			return UserDefn(iTmp);
		}
		else
			gAssert(false);
	}

	return 0;
}
	
#endif // TRACING


/*----------------------------------------------------------------------------------------------
	Write out the final underlying-to-surface associations.
----------------------------------------------------------------------------------------------*/
void Segment::LogXmlUnderlyingToSurface(std::ostream & strmOut, GrTableManager * ptman,
	GrCharStream * pchstrm, int nIndent)
{
#ifdef TRACING

	ptman->LogXmlTagOpen(strmOut, "UnderlyingToSurface", nIndent, true);
	ptman->LogXmlTagPostAttrs(strmOut, true);

	ptman->LogXmlUnderlyingAux(strmOut, pchstrm, -m_ichwAssocsMin, (m_ichwAssocsLim - m_ichwAssocsMin),
		nIndent+1,
		true,	// text
		false,	// features
		false,	// color
		true,	// string offset
		true,	// ligature components
		true	// glyph associations
	);

	ptman->LogXmlTagClose(strmOut, "UnderlyingToSurface", nIndent, true);


#ifdef XMLOMIT
	ptman->LogXmlTagOpen(strmOut, "UnderlyingToSurface", nIndent, true);

	size_t cassocs = 0;
	int fLigs = false;
	int ichw;
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		if (m_prgpvisloutAssocs[ichw])
			cassocs = max(cassocs, m_prgpvisloutAssocs[ichw]->size());
		if (m_prgisloutLigature[ichw] != kNegInfinity)
			fLigs = true;
	}

	ptman->LogUnderlyingHeader(strmOut, pchstrm->Min(), (pchstrm->Min() + m_ichwAssocsLim),
		-m_ichwAssocsMin, NULL);

	int rgnChars[MAX_SLOTS];
	bool rgfNewRun[MAX_SLOTS];
	std::fill_n(rgfNewRun, MAX_SLOTS, false);
	GrFeatureValues rgfval[MAX_SLOTS];
	int cchwMaxRawChars;

	int cchw = pchstrm->GetLogData(ptman, rgnChars, rgfNewRun, rgfval,
		-m_ichwAssocsMin, &cchwMaxRawChars);
	cchw = min(cchw, MAX_SLOTS);

	utf16 rgchwChars2[MAX_SLOTS];
	utf16 rgchwChars3[MAX_SLOTS];
	utf16 rgchwChars4[MAX_SLOTS];
	utf16 rgchwChars5[MAX_SLOTS];
	utf16 rgchwChars6[MAX_SLOTS];
	int rgcchwRaw[MAX_SLOTS];
	if (cchwMaxRawChars > 1)
	{
		cchwMaxRawChars = min(cchwMaxRawChars, 6);
		pchstrm->GetLogDataRaw(ptman, cchw, -m_ichwAssocsMin, cchwMaxRawChars,
			rgnChars, rgchwChars2, rgchwChars3, rgchwChars4, rgchwChars5, rgchwChars6,
			rgcchwRaw);
	}
	else
	{
		for (ichw = 0 ; ichw <(m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			rgcchwRaw[ichw] = 1;
			rgchwChars2[ichw] = 0;
			rgchwChars3[ichw] = 0;
			rgchwChars4[ichw] = 0;
			rgchwChars5[ichw] = 0;
			rgchwChars6[ichw] = 0;
		}
	}

	//	Text
	strmOut << "Text:          ";	// 15 spaces
	int inUtf32 = 0;
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		utf16 chw, chwNext;
		switch (rgcchwRaw[ichw])
		{
		default:
		case 1:	chw = utf16(rgnChars[inUtf32]); chwNext = rgchwChars2[inUtf32];	break;
		case 2:	chw = rgchwChars2[inUtf32]; chwNext = rgchwChars3[inUtf32];	break;
		case 3:	chw = rgchwChars3[inUtf32]; chwNext = rgchwChars4[inUtf32];	break;
		case 4:	chw = rgchwChars4[inUtf32]; chwNext = rgchwChars5[inUtf32];	break;
		case 5:	chw = rgchwChars5[inUtf32]; chwNext = rgchwChars6[inUtf32];	break;
		case 6:	chw = rgchwChars6[inUtf32]; chwNext = 0;					break;
		}
		if (rgcchwRaw[ichw] == 1 && chwNext == 0 && chw < 0x0100) // ANSI
			strmOut << (char)chw << "      ";	// 6 spaces
		else
			strmOut << "       "; // 7 spaces
		if (chwNext == 0)
			inUtf32++;
	}
	strmOut << "\n";

	//	Unicode
	strmOut << "Unicode:       ";
	inUtf32 = 0;
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		utf16 chw, chwNext;
		switch (rgcchwRaw[ichw])
		{
		default:
		case 1:	chw = utf16(rgnChars[inUtf32]); chwNext = rgchwChars2[inUtf32];	break;
		case 2:	chw = rgchwChars2[inUtf32]; chwNext = rgchwChars3[inUtf32];	break;
		case 3:	chw = rgchwChars3[inUtf32]; chwNext = rgchwChars4[inUtf32];	break;
		case 4:	chw = rgchwChars4[inUtf32]; chwNext = rgchwChars5[inUtf32];	break;
		case 5:	chw = rgchwChars5[inUtf32]; chwNext = rgchwChars6[inUtf32];	break;
		case 6:	chw = rgchwChars6[inUtf32]; chwNext = 0;					break;
		}
		ptman->LogHexInTable(strmOut, chw, chwNext != 0);
		if (chwNext == 0)
			inUtf32++;
	}
	strmOut << "\n";

	strmOut << "before         ";
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		if (rgcchwRaw[ichw] > 1)
			// continuation of Unicode codepoint
			strmOut << "       ";
		else if (m_prgisloutBefore[ichw] == kNegInfinity)
			strmOut << "<--    ";
		else if (m_prgisloutBefore[ichw] == kPosInfinity)
			strmOut << "-->    ";
		else
			ptman->LogInTable(strmOut, m_prgisloutBefore[ichw]);
	}
	strmOut <<"\n";

	for (int ix = 1; ix < signed(cassocs) - 1; ix++) //(cassocs > 2)
	{
		if (ix == 1)
			strmOut << "other          ";
		else
			strmOut << "               ";
		for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			std::vector<int> * pvislout = m_prgpvisloutAssocs[ichw];
			if (pvislout == NULL)
				strmOut << "       ";
			else if (signed(pvislout->size()) <= ix)
				strmOut << "       ";
			else if ((*pvislout)[ix] != m_prgisloutAfter[ichw])
				ptman->LogInTable(strmOut, (*pvislout)[ix]);
			else
				strmOut << "       ";
		}
		strmOut << "\n";
	}

	strmOut << "after          ";
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		if (rgcchwRaw[ichw] > 1)
			// continuation of Unicode codepoint
			strmOut << "       ";
		else if (m_prgisloutAfter[ichw] == kNegInfinity)
			strmOut << "<--    ";
		else if (m_prgisloutAfter[ichw] == kPosInfinity)
			strmOut << "-->    ";
		else
			ptman->LogInTable(strmOut, m_prgisloutAfter[ichw]);
	}
	strmOut <<"\n";

	if (fLigs)
	{
		strmOut << "ligature       ";
		for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			if (rgcchwRaw[ichw] > 1)
				// continuation of Unicode codepoint
				strmOut << "       ";
			else if (m_prgisloutLigature[ichw] != kNegInfinity)
				ptman->LogInTable(strmOut, m_prgisloutLigature[ichw]);
			else
				strmOut << "       ";
		}
		strmOut << "\n";

		strmOut << "component      ";
		for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			if (rgcchwRaw[ichw] > 1)
				// continuation of Unicode codepoint
				strmOut << "       ";
			else if (m_prgisloutLigature[ichw] != kNegInfinity)
				ptman->LogInTable(strmOut, m_prgiComponent[ichw] + 1);	// 1-based
			else
				strmOut << "       ";
		}
		strmOut << "\n";
	}

	strmOut << "\n";
#endif // XMLOMIT
#endif // TRACING

}

#ifdef XMLOMIT

/*----------------------------------------------------------------------------------------------
	Write out the final surface-to-underlying associations.
----------------------------------------------------------------------------------------------*/
void Segment::LogSurfaceToUnderlying(GrTableManager * ptman, std::ostream & strmOut)
{
#ifdef TRACING
	strmOut << "\nSURFACE TO UNDERLYING MAPPINGS\n\n";

	ptman->LogSlotHeader(strmOut, m_cslout, SP_PER_SLOT, LEADING_SP);

	int ccomp = 0;

	strmOut << "Glyph IDs:     ";
	int islout;
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << "#      ";
		}
		else
		{
			ptman->LogHexInTable(strmOut, psloutTmp->GlyphID());
			ccomp = max(ccomp, psloutTmp->NumberOfComponents());
		}
	}
	strmOut << "\n";

	bool fAnyPseudos = false;
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->GlyphID() != psloutTmp->ActualGlyphForOutput(ptman))
		{
			fAnyPseudos = true;
			break;
		}
	}
	if (fAnyPseudos)
	{
		strmOut << "Actual glyphs: ";
		for (int islout = 0; islout < m_cslout; islout++)
		{
			GrSlotOutput * psloutTmp = m_prgslout + islout;
			if (psloutTmp->GlyphID() != psloutTmp->ActualGlyphForOutput(ptman))
				ptman->LogHexInTable(strmOut, psloutTmp->ActualGlyphForOutput(ptman));
			else
				strmOut << "       ";
		}
		strmOut << "\n";
	}

	strmOut << "before         ";
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << "       ";
		}
		else
			ptman->LogInTable(strmOut, psloutTmp->BeforeAssoc());
	}
	strmOut << "\n";

	strmOut << "after          ";
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << "       ";
		}
		else
			ptman->LogInTable(strmOut, psloutTmp->AfterAssoc());
	}
	strmOut << "\n";

	for (int icomp = 0; icomp < ccomp; icomp++)
	{
		strmOut << "component " << icomp + 1	// 1=based
			<< "    ";
		for (islout = 0; islout < m_cslout; islout++)
		{
			GrSlotOutput * psloutTmp = m_prgslout + islout;
			if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
				psloutTmp->SpecialSlotFlag() == kspslLbFinal)
			{
				strmOut << "       ";
			}
			else if (icomp < psloutTmp->NumberOfComponents())
				ptman->LogInTable(strmOut, psloutTmp->UnderlyingComponent(icomp));
			else
				strmOut << "       ";
		}
		strmOut << "\n";
	}
#endif // TRACING
}

#ifdef TRACING
/*----------------------------------------------------------------------------------------------
	Log the value of the association, if the slot changed.
----------------------------------------------------------------------------------------------*/
void GrSlotState::LogAssociation(GrTableManager * ptman,
	std::ostream & strmOut, int ipass, int iassoc, bool fBoth, bool fAfter)
{
	if (m_ipassModified != ipass)
	{
		strmOut << "       ";
		return;
	}

	if (fBoth)
	{
		GrSlotState * pslotBefore = (m_vpslotAssoc.size()) ? m_vpslotAssoc[0] : NULL;
		GrSlotState * pslotAfter = (m_vpslotAssoc.size()) ? m_vpslotAssoc.back() : NULL;
		// handle possible reprocessing
		while (pslotBefore && pslotBefore->PassModified() == m_ipassModified)
			pslotBefore = pslotBefore->m_pslotPrevState;
		while (pslotAfter && pslotAfter->PassModified() == m_ipassModified)
			pslotAfter = pslotAfter->m_pslotPrevState;

		int nBefore, nAfter;


		int csp = 4;
		if (pslotBefore)
		{
			nBefore = pslotBefore->m_islotTmpIn;
			strmOut << nBefore;
			if (nBefore > 99) csp--;
			if (nBefore > 9) csp--;
		}
		else
		{
			strmOut << "??";
			csp--;
		}
		if (pslotAfter)
		{
			nAfter = pslotAfter->m_islotTmpIn;
			strmOut << "/" << nAfter;
			if (nAfter > 99) csp--;
			if (nAfter > 9) csp--;
		}
		else
		{
			if (pslotBefore)
			{
				strmOut << "/" << "??";
				csp--;
			}
			else
				csp = 5;
		}

		for (int isp = 0; isp < csp; isp++)
			strmOut << " ";
	}

	else if (fAfter)
	{
		Assert(m_vpslotAssoc.size());
		GrSlotState * pslotAfter = m_vpslotAssoc.back();
		// handle possible reprocessing
		while (pslotAfter && pslotAfter->PassModified() == m_ipassModified)
			pslotAfter = pslotAfter->m_pslotPrevState;

		if (pslotAfter)
		{
			int nAfter = pslotAfter->m_islotTmpIn;
			ptman->LogInTable(strmOut, nAfter);
		}
		else
			strmOut << "??     ";
	}
	else if (iassoc < signed(m_vpslotAssoc.size()))
	{
		GrSlotState * pslot = m_vpslotAssoc[iassoc];
		// handle possible reprocessing
		while (pslot && pslot->PassModified() == m_ipassModified)
			pslot = pslot->m_pslotPrevState;

		if (pslot)
		{
			int n = pslot->m_islotTmpIn;
			ptman->LogInTable(strmOut, n);
		}
		else
			strmOut << "??     ";
	}
	else
		strmOut << "       ";
}


/*----------------------------------------------------------------------------------------------
	Write a directionality code to the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogDirCodeInTable(std::ostream & strmOut, int dirc)
{
	switch (dirc)
	{
	case kdircUnknown:		strmOut << "???    "; break;
	case kdircNeutral:		strmOut << "ON     "; break;
	case kdircL:			strmOut << "L      "; break;
	case kdircR:			strmOut << "R      "; break;
	case kdircRArab:		strmOut << "AR     "; break;
	case kdircEuroNum:		strmOut << "EN     "; break;
	case kdircEuroSep:		strmOut << "ES     "; break;
	case kdircEuroTerm:		strmOut << "ET     "; break;
	case kdircArabNum:		strmOut << "AN     "; break;
	case kdircComSep:		strmOut << "CS     "; break;
	case kdircWhiteSpace:	strmOut << "WS     "; break;
	case kdircBndNeutral:	strmOut << "BN     "; break;
	case kdircNSM:			strmOut << "NSM    "; break;
	case kdircLRO:			strmOut << "LRO    "; break;
	case kdircRLO:			strmOut << "RLO    "; break;
	case kdircLRE:			strmOut << "LRE    "; break;
	case kdircRLE:			strmOut << "RLE    "; break;
	case kdircPDF:			strmOut << "PDF    "; break;
	case kdircPdfL:			strmOut << "PDF-L  "; break;
	case kdircPdfR:			strmOut << "PDF-R  "; break;
	case kdircLlb:			strmOut << "L      "; break;
	case kdircRlb:			strmOut << "R      "; break;
	default:				LogInTable(strmOut, dirc); break;
	}
}

/*----------------------------------------------------------------------------------------------
	Write a breakweight code to the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogBreakWeightInTable(std::ostream & strmOut, int lb)
{
	if (lb < 0)
	{
		lb = lb * -1;
		switch (lb)
		{
		case klbWsBreak:		strmOut << "-ws    "; break;
		case klbWordBreak:		strmOut << "-word  "; break;
		case klbHyphenBreak:	strmOut << "-intra "; break;
		case klbLetterBreak:	strmOut << "-lettr "; break;
		case klbClipBreak:		strmOut << "-clip  "; break;
		default:				LogInTable(strmOut, lb); break;
		}
	}
	else
	{
		switch (lb)
		{
		case klbNoBreak:		strmOut << "none   "; break;
		case klbWsBreak:		strmOut << "ws     "; break;
		case klbWordBreak:		strmOut << "word   "; break;
		case klbHyphenBreak:	strmOut << "intra  "; break;
		case klbLetterBreak:	strmOut << "letter "; break;
		case klbClipBreak:		strmOut << "clip   "; break;
		default:				LogInTable(strmOut, lb); break;
		}
	}
}

#endif // TRACING

#endif // XMLOMIT

#ifdef TRACING
/*----------------------------------------------------------------------------------------------
	Write an XML tag, leaving it open to possibly add attributes.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlTagOpen(std::ostream & strmOut, std::string strTag, size_t nIndent,
	bool fContent)
{
	for (size_t i = 0; i < nIndent; i++)
		strmOut << "  ";

	strmOut << "<" << strTag;
}

/*----------------------------------------------------------------------------------------------
	Write the termination of the opening XML tag.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlTagPostAttrs(std::ostream & strmOut,
	bool fContent)
{
	if (fContent)
		strmOut << ">\n";
}

/*----------------------------------------------------------------------------------------------
	Write the closing form of the XML tag.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlTagClose(std::ostream & strmOut, std::string strTag, size_t nIndent,
	 bool fContent)
{
	if (fContent)
	{
		for (size_t i = 0; i < nIndent; i++)
			strmOut << "  ";
		strmOut << "</" << strTag << ">\n";
	}
	else
	{
		strmOut << " />\n";
	}
}

/*----------------------------------------------------------------------------------------------
	Write an attribute/value pair inside of a tag.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlTagAttr(std::ostream & strmOut, std::string strAttr, int nValue,
	size_t nIndent)
{
	char chBuf[20];
	itoa(nValue, chBuf, 10);
	LogXmlTagAttr(strmOut, strAttr, chBuf, nIndent);
}

void GrTableManager::LogXmlTagAttr(std::ostream & strmOut, std::string strAttr, const char * szValue,
	size_t nIndent)
{
	if (nIndent > 0)
	{
		strmOut << "\n";
		for (size_t i = 0; i < nIndent + 2; i++)
			strmOut << "  ";
	}
	else
		strmOut << " ";

	strmOut << strAttr << "=\"" << szValue << "\"";
}

void GrTableManager::LogXmlTagAttrHex(std::ostream & strmOut, std::string strAttr, int nValue,
	size_t nIndent)
{
	char chBuf[20];
	chBuf[0] = chBuf[1] = chBuf[2] = '0';
	if (nValue < 0x10)
		itoa(nValue, chBuf+3, 16);	// prepend 3 zeros
	else if (nValue < 0x100)
		itoa(nValue, chBuf+2, 16);	// prepend 2 zeros
	else if (nValue < 0x1000)
		itoa(nValue, chBuf+1, 16);	// prepend 1 zero
	else
		itoa(nValue, chBuf, 16);
	LogXmlTagAttr(strmOut, strAttr, chBuf, nIndent);
}

/*----------------------------------------------------------------------------------------------
	Concatenate the hex form of the given numbers into a string.
----------------------------------------------------------------------------------------------*/
std::string GrTableManager::HexString(std::vector<int> vn)
{
	std::string strRet;
	for (size_t in = 0; in < vn.size(); in++)
	{
		char rgch[20];
		itoa(vn[in],rgch,16);
		strRet.append(rgch);
		if (in < vn.size() - 1)
			strRet.append(" ");
	}
	return strRet;
}

std::string GrTableManager::HexString(int n1, int n2, int n3, int n4, int n5, int n6)
{
	std::vector<int> vn;
	if (n1 > 0)
	{
		vn.push_back(n1);
		if (n2 > 0)
		{
			vn.push_back(n2);
			if (n3 > 0)
			{
				vn.push_back(n3);
				if (n4 > 0)
				{
					vn.push_back(n4);
					if (n5 > 0)
					{
						vn.push_back(n5);
						if (n6 > 0)
							vn.push_back(n6);
					}
				}
			}
		}
	}
	return HexString(vn);
}

/*----------------------------------------------------------------------------------------------
	Write an attribute/value pair inside of a tag.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlTagColor(std::ostream & strmOut, std::string strAttr, int clrValue,
	bool fBack, size_t nIndent)
{
	// Ignore black foreground and white background.
	if (!fBack && clrValue == kclrBlack)
		return;
	if (fBack && (clrValue == kclrWhite || clrValue == kclrTransparent))
		return;

	std::string strValue;
	char rgch[20];
	switch (clrValue)
	{
	case kclrBlack:	strValue = "black";		break;
	case kclrWhite:	strValue = "white";		break;
	case kclrRed:	strValue = "red";		break;
	case kclrGreen:	strValue = "green";		break;
	case kclrBlue:	strValue = "blue";		break;
	case 0x00ffff:	strValue = "yellow";	break;
	case 0xff00ff:	strValue = "magenta";	break;
	case 0xffff00:	strValue = "cyan";		break;
	default:
		rgch[0] = '#';
		itoa(clrValue, rgch+1, 16);
		strValue.assign(rgch);
		break;
	}

	LogXmlTagAttr(strmOut, strAttr, strValue.c_str(), nIndent);
}

/*----------------------------------------------------------------------------------------------
	Write a directionality code to the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlDirCode(std::ostream & strmOut, std::string strAttr, int dircValue,
	size_t nIndent)
{
	std::string strValue;
	char rgch[20];
	switch (dircValue)
	{
	case kdircUnknown:		strValue = "unknown";	break;
	case kdircNeutral:		strValue = "ON";	break;
	case kdircL:			strValue = "L";		break;
	case kdircR:			strValue = "R";		break;
	case kdircRArab:		strValue = "AR";	break;
	case kdircEuroNum:		strValue = "EN";	break;
	case kdircEuroSep:		strValue = "ES";	break;
	case kdircEuroTerm:		strValue = "ET";	break;
	case kdircArabNum:		strValue = "AN";	break;
	case kdircComSep:		strValue = "CS";	break;
	case kdircWhiteSpace:	strValue = "WS";	break;
	case kdircBndNeutral:	strValue = "BN";	break;
	case kdircNSM:			strValue = "NSM";	break;
	case kdircLRO:			strValue = "LRO";	break;
	case kdircRLO:			strValue = "RLO";	break;
	case kdircLRE:			strValue = "LRE";	break;
	case kdircRLE:			strValue = "RLE";	break;
	case kdircPDF:			strValue = "PDF";	break;
	case kdircPdfL:			strValue = "PDF-L";	break;
	case kdircPdfR:			strValue = "PDF-R";	break;
	case kdircLlb:			strValue = "L";		break;
	case kdircRlb:			strValue = "R";		break;
	default:
		itoa(dircValue, rgch, 10);
		strValue.assign(rgch);
		break;
	}
	LogXmlTagAttr(strmOut, strAttr, strValue.c_str(), nIndent);
}

/*----------------------------------------------------------------------------------------------
	Write a breakweight code to the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlBreakWeight(std::ostream & strmOut, std::string strAttr, int lbValue,
	size_t nIndent)
{
	std::string strValue;
	char rgch[20];
	if (lbValue < 0)
	{
		lbValue = lbValue * -1;
		switch (lbValue)
		{
		case klbWsBreak:		strValue = "-ws";		break;
		case klbWordBreak:		strValue = "-word";		break;
		case klbHyphenBreak:	strValue = "-intra";	break;
		case klbLetterBreak:	strValue = "-letter";	break;
		case klbClipBreak:		strValue = "-clip";		break;
		default:
			itoa(lbValue * -1, rgch, 10);
			strValue.assign(rgch);
			break;
		}
	}
	else
	{
		switch (lbValue)
		{
		case klbNoBreak:		strValue = "none";		break;
		case klbWsBreak:		strValue = "ws";		break;
		case klbWordBreak:		strValue = "word";		break;
		case klbHyphenBreak:	strValue = "intra";		break;
		case klbLetterBreak:	strValue = "letter";	break;
		case klbClipBreak:		strValue = "clip";		break;
		default:
			itoa(lbValue, rgch, 10);
			strValue.assign(rgch);
			break;
		}
	}
	LogXmlTagAttr(strmOut, strAttr, strValue.c_str(), nIndent);
}

/*----------------------------------------------------------------------------------------------
	Add a comment to the log file.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogXmlComment(std::ostream & strmOut, std::string strComment,
	size_t nIndent)
{
	for (size_t i = 0; i < nIndent; i++)
		strmOut << "  ";

	strmOut << "<!-- " << strComment << " -->\n";
}


#endif // TRACING

} // namespace gr
