/*
 *******************************************************************************
 *
 *   Copyright (C) 1999-2008, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  gendata.cpp
 *
 *   created on: 11/03/2000
 *   created by: Eric R. Mader
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/uscript.h"
#include "unicode/ubidi.h"

#include "layout/LETypes.h"
#include "layout/LEScripts.h"
#include "layout/LayoutEngine.h"

#include "PortableFontInstance.h"
#include "SimpleFontInstance.h"

#include "xmlparser.h"

#include "letsutil.h"
#include "letest.h"

U_NAMESPACE_USE

struct TestInput
{
    const char *fontName;
    LEUnicode  *text;
    le_int32    textLength;
    le_int32    scriptCode;
    le_bool     rightToLeft;
};

/* 
 * FIXME: should use the output file name and the current date.
 */
const char *header =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "\n"
    "<!--\n"
    "  Copyright (c) 1999-%4.4d International Business Machines\n"
    "  Corporation and others. All rights reserved.\n"
    "\n"
    "  WARNING: THIS FILE IS MACHINE GENERATED. DO NOT HAND EDIT IT\n"
    "  UNLESS YOU REALLY KNOW WHAT YOU'RE DOING.\n"
    "\n"
    "  file name:    letest.xml\n"
    "  generated on: %s\n"
    "  generated by: gendata.cpp\n"
    "-->\n"
    "\n"
    "<layout-tests>\n";

void dumpLongs(FILE *file, const char *tag, le_int32 *longs, le_int32 count) {
    char lineBuffer[8 * 12 + 2];
    le_int32 bufp = 0;

    fprintf(file, "        <%s>\n", tag);

    for (int i = 0; i < count; i += 1) {
        if (i % 8 == 0 && bufp != 0) {
            fprintf(file, "            %s\n", lineBuffer);
            bufp = 0;
        }

        bufp += sprintf(&lineBuffer[bufp], "0x%8.8X, ", longs[i]);
    }

    if (bufp != 0) {
        lineBuffer[bufp - 2] = '\0';
        fprintf(file, "            %s\n", lineBuffer);
    }

    fprintf(file, "        </%s>\n\n", tag);
}

void dumpFloats(FILE *file, const char *tag, float *floats, le_int32 count) {
    char lineBuffer[8 * 16 + 2];
    le_int32 bufp = 0;

    fprintf(file, "        <%s>\n", tag);

    for (int i = 0; i < count; i += 1) {
        if (i % 8 == 0 && bufp != 0) {
            fprintf(file, "            %s\n", lineBuffer);
            bufp = 0;
        }

        bufp += sprintf(&lineBuffer[bufp], "%f, ", floats[i]);
    }

    if (bufp != 0) {
        lineBuffer[bufp - 2] = '\0';
        fprintf(file, "            %s\n", lineBuffer);
    }

    fprintf(file, "        </%s>\n", tag);
}

int main(int /*argc*/, char *argv[])
{
    UErrorCode status = U_ZERO_ERROR;
    FILE *outputFile = fopen(argv[1], "w");
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    const char *tmFormat = "%m/%d/%Y %I:%M:%S %p %Z";
    char  tmString[64];

    strftime(tmString, 64, tmFormat, local);
    fprintf(outputFile, header, local->tm_year + 1900, tmString);

    UXMLParser  *parser = UXMLParser::createParser(status);
    UXMLElement *root   = parser->parseFile("gendata.xml", status);

    if (root == NULL) {
        printf("Error: Could not open gendata.xml\n");
        delete parser;
        return -1;
    }

    UnicodeString test_case        = UNICODE_STRING_SIMPLE("test-case");
    UnicodeString test_text        = UNICODE_STRING_SIMPLE("test-text");
    UnicodeString test_font        = UNICODE_STRING_SIMPLE("test-font");

    // test-case attributes
    UnicodeString id_attr     = UNICODE_STRING_SIMPLE("id");
    UnicodeString script_attr = UNICODE_STRING_SIMPLE("script");
    UnicodeString lang_attr   = UNICODE_STRING_SIMPLE("lang");

    // test-font attributes
    UnicodeString name_attr   = UNICODE_STRING_SIMPLE("name");

    const UXMLElement *testCase;
    int32_t tc = 0;

    while((testCase = root->nextChildElement(tc)) != NULL) {
        if (testCase->getTagName().compare(test_case) == 0) {
            char *id = getCString(testCase->getAttribute(id_attr));
            char *script = getCString(testCase->getAttribute(script_attr));
            char *lang   = getCString(testCase->getAttribute(lang_attr));
            LEFontInstance *font = NULL;
            const UXMLElement *element;
            int32_t ec = 0;
            int32_t charCount = 0;
            int32_t typoFlags = 3; // kerning + ligatures...
            UScriptCode scriptCode;
            le_int32 languageCode = -1;
            UnicodeString text;
            int32_t glyphCount = 0;
            LEErrorCode leStatus = LE_NO_ERROR;
            LayoutEngine *engine = NULL;
            LEGlyphID *glyphs    = NULL;
            le_int32  *indices   = NULL;
            float     *positions = NULL;

            uscript_getCode(script, &scriptCode, 1, &status);
            if (LE_FAILURE(status)) {
                printf("Error: invalid script name: %s.\n", script);
                goto free_c_strings;
            }

            if (lang != NULL) {
                languageCode = getLanguageCode(lang);

                if (languageCode < 0) {
                    printf("Error: invalid language name: %s.\n", lang);
                    goto free_c_strings;
                }

                fprintf(outputFile, "    <test-case id=\"%s\" script=\"%s\" lang=\"%s\">\n", id, script, lang);
            } else {
                fprintf(outputFile, "    <test-case id=\"%s\" script=\"%s\">\n", id, script);
            }

            while((element = testCase->nextChildElement(ec)) != NULL) {
                UnicodeString tag = element->getTagName();

                // TODO: make sure that each element is only used once.
                if (tag.compare(test_font) == 0) {
                    char *fontName  = getCString(element->getAttribute(name_attr));
                    const char *version = NULL;
                    PortableFontInstance *pfi = new PortableFontInstance(fontName, 12, leStatus);
                    
                    if (LE_FAILURE(leStatus)) {
                        printf("Error: could not open font: %s\n", fontName);
                        freeCString(fontName);
                        goto free_c_strings;
                    }

                    version = pfi->getNameString(NAME_VERSION_STRING, PLATFORM_MACINTOSH, MACINTOSH_ROMAN, MACINTOSH_ENGLISH);

                    // The standard recommends that the Macintosh Roman/English name string be present, but
                    // if it's not, try the Microsoft Unicode/English string.
                    if (version == NULL) {
                        const LEUnicode16 *uversion = pfi->getUnicodeNameString(NAME_VERSION_STRING, PLATFORM_MICROSOFT, MICROSOFT_UNICODE_BMP, MICROSOFT_ENGLISH);

                        if (uversion != NULL) {
                            const UnicodeString usversion((const UChar *)uversion);

                            version = getCString(&usversion);
                            pfi->deleteNameString(uversion);
                        }
                    }

                    fprintf(outputFile, "        <test-font name=\"%s\" version=\"%s\" checksum=\"0x%8.8X\"/>\n\n",
                        fontName, version, pfi->getFontChecksum());

                    pfi->deleteNameString(version);
                    freeCString(fontName);
                    font = pfi;
                } else if (tag.compare(test_text) == 0) {
                    char *utf8 = NULL;

                    text = element->getText(TRUE);
                    charCount = text.length();

                    utf8 = getUTF8String(&text);
                    fprintf(outputFile, "        <test-text>%s</test-text>\n\n", utf8);
                    freeCString(utf8);
                } else {
                    // an unknown tag...
                    char *cTag = getCString(&tag);

                    printf("Test %s: unknown element with tag \"%s\"\n", id, cTag);
                    freeCString(cTag);
                }
            }

            if (font == NULL) {
                LEErrorCode fontStatus = LE_NO_ERROR;

                font = new SimpleFontInstance(12, fontStatus);
                typoFlags |= 0x80000000L;  // use CharSubstitutionFilter...
            }

            engine = LayoutEngine::layoutEngineFactory(font, scriptCode, languageCode, typoFlags, leStatus);

            if (LE_FAILURE(leStatus)) {
                printf("Error for test %s: could not create a LayoutEngine.\n", id);
                goto delete_font;
            }

            glyphCount = engine->layoutChars(text.getBuffer(), 0, charCount, charCount, getRTL(text), 0, 0, leStatus);

            glyphs    = NEW_ARRAY(LEGlyphID, glyphCount);
            indices   = NEW_ARRAY(le_int32, glyphCount);
            positions = NEW_ARRAY(float, glyphCount * 2 + 2);

            engine->getGlyphs(glyphs, leStatus);
            engine->getCharIndices(indices, leStatus);
            engine->getGlyphPositions(positions, leStatus);

            dumpLongs(outputFile, "result-glyphs", (le_int32 *) glyphs, glyphCount);

            dumpLongs(outputFile, "result-indices", indices, glyphCount);

            dumpFloats(outputFile, "result-positions", positions, glyphCount * 2 + 2);

            fprintf(outputFile, "    </test-case>\n\n");

            DELETE_ARRAY(positions);
            DELETE_ARRAY(indices);
            DELETE_ARRAY(glyphs);

            delete engine;

delete_font:
            delete font;

free_c_strings:
            freeCString(lang);
            freeCString(script);
            freeCString(id);
        }
    }

    delete root;
    delete parser;

    fprintf(outputFile, "</layout-tests>\n");

    fclose(outputFile);
}
