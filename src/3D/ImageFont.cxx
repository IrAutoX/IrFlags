/* bzflag
 * Copyright (c) 1993-2025 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "ImageFont.h"
#include <string>
#include <string.h>
#include "bzfgl.h"
#include "bzfio.h"
#include "MediaFile.h"
#include "PersianText.h"

ImageFont::ImageFont()
{
    for (int i = 0; i < MAX_TEXTURE_FONT_CHARS; i++)
        fontMetrics[i].charWidth = -1;
    size = -1;
    textureXSize = -1;
    textureYSize = -1;
    textureZStep = -1;
    numberOfCharacters = -1;
}

ImageFont::~ImageFont() {}
int ImageFont::getSize() const { return size; }
const char* ImageFont::getFaceName() const { return faceName.c_str(); }

static int line;

bool readKeyInt(OSFile &file, std::string expectedLeft, int &retval, bool newfile=false)
{
    if (newfile) line = 0;
    const int expsize = int(expectedLeft.size());
    std::string tmpBuf;
    while (tmpBuf.size() == 0 || tmpBuf[0] == '#' || tmpBuf[0] == 10 || tmpBuf[0] == 13)
    {
        tmpBuf = file.readLine();
        line++;
    }
    if (tmpBuf.substr(0, expsize) == expectedLeft && tmpBuf[expsize]==':')
    {
        long int retvalue = strtol(tmpBuf.c_str() + expsize + 1, (char **) NULL, 10);
        if (retvalue < INT_MIN || retvalue > INT_MAX) return false;
        retval = (int) retvalue;
        return true;
    }
    logDebugMessage(2,"Unexpected line in font metrics file %s, line %d (expected %s)\n",
                    file.getFileName().c_str(), line, expectedLeft.c_str());
    return false;
}

bool readLetter(OSFile &file, char expected)
{
    const std::string expectedLeft = "Char:";
    const int expsize = int(expectedLeft.size());
    std::string tmpBuf;
    while (tmpBuf.size() == 0 || tmpBuf[0] == '#' || tmpBuf[0] == 10 || tmpBuf[0] == 13)
    {
        tmpBuf = file.readLine();
        line++;
    }
    if (tmpBuf.substr(0, expsize) == expectedLeft)
    {
        if (int(tmpBuf.size()) >= expsize+4 && tmpBuf[expsize+1]=='"' && tmpBuf[expsize+3]=='"' &&
                tmpBuf[expsize+2]==expected)
            return true;
        logDebugMessage(2,"Unexpected character in font metrics file %s, line %d.\n",
                        file.getFileName().c_str(), line);
        return false;
    }
    logDebugMessage(2,"Unexpected line in font metrics file %s, line %d (expected %s)\n",
                    file.getFileName().c_str(), line, expectedLeft.c_str());
    return false;
}

bool ImageFont::load(OSFile &file)
{
    std::string extension = file.getExtension();
    if (extension=="") return false;
    texture = file.getFileName();
    std::string::size_type underscore = texture.rfind('_');
    if (underscore == std::string::npos)
    {
        logDebugMessage(1,"Unexpected font file name: %s, no _size found\n", file.getStdName().c_str());
        return false;
    }
    faceName = texture.substr(0, underscore);
    size = strtol(texture.c_str() + underscore + 1, (char **)NULL, 10);
    if (!file.open("rb")) return false;
    if (!readKeyInt(file, "NumChars", numberOfCharacters, true)) return false;
    if (!readKeyInt(file, "TextureWidth", textureXSize)) return false;
    if (!readKeyInt(file, "TextureHeight", textureYSize)) return false;
    if (!readKeyInt(file, "TextZStep", textureZStep)) return false;
    if (numberOfCharacters > MAX_TEXTURE_FONT_CHARS)
    {
        logDebugMessage(1,"Too many characters (%i) in %s.\n", numberOfCharacters, file.getFileName().c_str());
        numberOfCharacters = MAX_TEXTURE_FONT_CHARS;
    }
    for (int i = 0; i < numberOfCharacters; i++)
    {
        if (!readLetter(file, static_cast<char>(i + 32))) return false;
        if (!readKeyInt(file, "InitialDist", fontMetrics[i].initialDist)) return false;
        if (!readKeyInt(file, "Width", fontMetrics[i].charWidth)) return false;
        if (!readKeyInt(file, "Whitespace", fontMetrics[i].whiteSpaceDist)) return false;
        if (!readKeyInt(file, "StartX", fontMetrics[i].startX)) return false;
        if (!readKeyInt(file, "EndX", fontMetrics[i].endX)) return false;
        if (!readKeyInt(file, "StartY", fontMetrics[i].startY)) return false;
        if (!readKeyInt(file, "EndY", fontMetrics[i].endY)) return false;
        fontMetrics[i].fullWidth = fontMetrics[i].initialDist + fontMetrics[i].charWidth + fontMetrics[i].whiteSpaceDist;
    }
    file.close();
    return (numberOfCharacters > 0);
}

float ImageFont::getStrLength(float scale, const char *str, int len) const
{
    if (!str || len <= 0) return 0.0f;
    const std::string prepared = IrFlagsPersian::prepare(std::string(str, static_cast<std::size_t>(len)));
    float totalLen = 0;
    for (std::size_t i = 0; i < prepared.size(); i++)
    {
        const unsigned int glyph = static_cast<unsigned char>(prepared[i]);
        int charToUse = 32;
        if (glyph >= 32 && glyph < static_cast<unsigned int>(numberOfCharacters + 32))
            charToUse = static_cast<int>(glyph);
        charToUse -= 32;
        totalLen += (float)(fontMetrics[charToUse].fullWidth);
    }
    return totalLen * scale;
}

float ImageFont::getStrLength(float scale, const std::string &str) const
{
    return getStrLength(scale, str.c_str(), (int)str.size());
}

// Local Variables: ***
// mode: C++ ***
// tab-width: 4 ***
// c-basic-offset: 4 ***
// indent-tabs-mode: nil ***
// End: ***
// ex: shiftwidth=4 tabstop=4
