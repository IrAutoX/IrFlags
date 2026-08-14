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

#ifndef _TEXTURE_FONT_H_
#define _TEXTURE_FONT_H_

#include "ImageFont.h"
#include "bzfgl.h"
#include "OpenGLGState.h"

class TextureFont final : public ImageFont
{
public:
    TextureFont();
    virtual ~TextureFont();

    void build() override;
    bool isBuilt() const override;

    void filter(bool dofilter) override;
    void drawString(float scale, GLfloat color[4], const char *str, int len) override;
    float getStrLength(float scale, const char *str, int len) const override;
    float getStrLength(float scale, const std::string &str) const override;

    void free() override;

private:
    struct TtfRuntime;

    void preLoadLists();
    bool ensureTtf() const;
    void drawTtf(float scale, GLfloat color[4], const char *str, int len);
    float getTtfLength(float scale, const char *str, int len) const;
    void clearTtfGlyphs();

    unsigned int listIDs[MAX_TEXTURE_FONT_CHARS];
    int textureID;
    OpenGLGState gstate;
    mutable TtfRuntime* ttf;
};

#endif //_TEXTURE_FONT_H_

// Local Variables: ***
// mode: C++ ***
// tab-width: 4 ***
// c-basic-offset: 4 ***
// indent-tabs-mode: nil ***
// End: ***
// ex: shiftwidth=4 tabstop=4
