/* Output from p2c 1.21alpha-07.Dec.93, the Pascal-to-C translator */
/* From input file "control.pas" */


#include "p2c.h"


#define CONTROL_G
#include "control.h"


#ifndef UTILITY_H
#include "utility.h"
#endif


/* enables Include: directive */
/* one-word shortcut is treated by simply inserting a blank */
/* detached form of shortcut allowed */
/* C: lines are taken into account */
/* U: lines are taken into account */
/* L: lines are taken into account */
/* non-melismatic eighth and shorter notes in vocal lines are unbeamed */
/* blind slurs are hidden */
/* U: lines synchronize with words and rests */
/* always insert duration into notes */
/* include macros in the count */
/* literally expand macros */
/* keep track of pitch and warn if unreal */
/* transform note words to canical form */
/* use solfa note names */
/* issue warnings even when default action is likely to be correct */
/* ignore all errors except fatal errors */
/* set instrument names */
/* report what is being done */
/* write all possible infomative messages */
typedef enum {
  noSuchFeature, FmultiFile, FsplitShortcut, FnewWordShortcut, FdoChords,
  FdoUptext, FdoLyrics, FunbeamVocal, FhideBlindSlurs, FuptextOnRests,
  FinsertDuration, FcountMacro, FexpandMacro, FcheckPitch, FrearrangeNote,
  FsolfaNoteNames, FpedanticWarnings, FignoreErrors, FinstrumentNames,
  FbeVerbose, FdebugMode
} feature;

typedef struct Tfeature {
  Char tag[31];
  boolean active, changed;
} Tfeature;


#define firstFeature    FmultiFile
#define lastFeature     FdebugMode


Static Tfeature feat[21] = {
  { "", false, false },
  { "multiFile", true, false },
  { "splitShortcut", true, false },
  { "newWordShortcut", true, false },
  { "doChords", true, false },
  { "doUptext", true, false },
  { "doLyrics", true, false },
  { "unbeamVocal", true, false },
  { "hideBlindSlurs", true, false },
  { "uptextOnRests", true, false },
  { "insertDuration", true, false },
  { "countMacro", false, false },
  { "expandMacro", false, false },
  { "checkPitch", true, false },
  { "rearrangeNote", true, false },
  { "solfaNoteNames", false, false },
  { "pedanticWarnings", false, false },
  { "ignoreErrors", false, false },
  { "instrumentNames", false, false },
  { "beVerbose", false, false },
  { "debugMode", false, false }
};


Void printFeatures(anyway)
boolean anyway;
{
  feature i;
  Tfeature *WITH;

  for (i = firstFeature; (long)i <= (long)lastFeature; i = (feature)((long)i + 1)) {
    WITH = &feat[(long)i];
    if (WITH->changed || anyway)
      printf("%s = %s\n", WITH->tag, WITH->active ? " TRUE" : "FALSE");
  }
}


Static feature featureNamed(s)
Char *s;
{
  feature i;

  for (i = firstFeature; (long)i <= (long)lastFeature; i = (feature)((long)i + 1)) {
    if (equalsIgnoreCase(s, feat[(long)i].tag))
      return i;
  }
  return noSuchFeature;
}


boolean setFeature(which, val)
Char *which;
boolean val;
{
  boolean Result = false;
  feature f;
  Tfeature *WITH;

  f = featureNamed(which);
  if (f != noSuchFeature) {
    WITH = &feat[(long)f];
    WITH->active = val;
    WITH->changed = true;
    Result = true;
  }
  if (f == FdebugMode && val)
    feat[(long)FbeVerbose].active = true;
  if (f == FbeVerbose && !val)
    feat[(long)FdebugMode].active = false;
  return Result;
}


Void mtxLevel(level)
Char *level;
{
  if (strcmp(level, "0.57") < 0) {
    setFeature("splitShortcut", false);
    setFeature("newWordShortcut", false);
  }
}


/* Feature functions.  To add a new feature "newFeature":
   1. Insert a new value "FnewFeature" in the declaration of type "feature".
   2. Insert an entry for it in array "feat".
   3. Copy the template below and change "FEATURE" into "newFeature".
   4. Copy the function header to the interface section.
   5. (Optional) Insert code into "mtxLevel" to enable/disable the feature.

function FEATURE: boolean;
begin FEATURE := feat[FFEATURE].active end;
*/

boolean checkPitch()
{
  return (feat[(long)FcheckPitch].active);
}


boolean countMacro()
{
  return (feat[(long)FcountMacro].active);
}


boolean expandMacro()
{
  return (feat[(long)FexpandMacro].active);
}


boolean insertDuration()
{
  return (feat[(long)FinsertDuration].active);
}


boolean rearrangeNote()
{
  return (feat[(long)FrearrangeNote].active);
}


boolean beVerbose()
{
  return (feat[(long)FbeVerbose].active);
}


boolean debugMode()
{
  return (feat[(long)FdebugMode].active);
}


boolean instrumentNames()
{
  return (feat[(long)FinstrumentNames].active);
}


boolean hideBlindSlurs()
{
  return (feat[(long)FhideBlindSlurs].active);
}


boolean doLyrics()
{
  return (feat[(long)FdoLyrics].active);
}


boolean ignoreErrors()
{
  return (feat[(long)FignoreErrors].active);
}


boolean pedanticWarnings()
{
  return (feat[(long)FpedanticWarnings].active);
}


boolean solfaNoteNames()
{
  return (feat[(long)FsolfaNoteNames].active);
}


boolean uptextOnRests()
{
  return (feat[(long)FuptextOnRests].active);
}


boolean unbeamVocal()
{
  return (feat[(long)FunbeamVocal].active);
}


boolean doChords()
{
  return (feat[(long)FdoChords].active);
}


boolean doUptext()
{
  return (feat[(long)FdoUptext].active);
}


boolean newWordShortcut()
{
  return (feat[(long)FnewWordShortcut].active);
}


boolean splitShortcut()
{
  return (feat[(long)FsplitShortcut].active);
}


boolean multiFile()
{
  return (feat[(long)FmultiFile].active);
}





/* End. */
