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

#include "common.h"
#include "ImageFont.h"
#include "TextureFont.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#endif

#include "bzfgl.h"
#include "bzfio.h"
#include "OpenGLGState.h"
#include "TextureManager.h"
#include "PersianText.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "../../third_party/stb/stb_truetype.h"

struct TextureFont::TtfRuntime
{
    struct Glyph
    {
        GLuint texture;
        int width;
        int height;
        int xoff;
        int yoff;
        float advance;

        Glyph() : texture(0), width(0), height(0), xoff(0), yoff(0), advance(0.0f) {}
    };

    bool attempted;
    bool ready;
    std::vector<unsigned char> bytes;
    stbtt_fontinfo font;
    std::map<std::uint32_t, Glyph> glyphs;
    std::string path;

    TtfRuntime() : attempted(false), ready(false) {}
};

TextureFont::TextureFont() : textureID(-1), ttf(new TtfRuntime)
{
    for (int i = 0; i < MAX_TEXTURE_FONT_CHARS; i++)
        listIDs[i] = INVALID_GL_LIST_ID;
}

TextureFont::~TextureFont()
{
    clearTtfGlyphs();
    delete ttf;
    ttf = NULL;

    for (int i = 0; i < MAX_TEXTURE_FONT_CHARS; i++)
    {
        if (listIDs[i] != INVALID_GL_LIST_ID)
        {
            glDeleteLists(listIDs[i], 1);
            listIDs[i] = INVALID_GL_LIST_ID;
        }
    }
}

bool TextureFont::ensureTtf() const
{
    if (!ttf)
        return false;
    if (ttf->ready)
        return true;
    if (ttf->attempted)
        return false;

    ttf->attempted = true;

    std::vector<std::string> candidates;
#ifdef _WIN32
    char modulePath[MAX_PATH + 1] = {0};
    const DWORD moduleLen = GetModuleFileNameA(NULL, modulePath, MAX_PATH);
    if (moduleLen > 0 && moduleLen < MAX_PATH)
    {
        std::string dir(modulePath, moduleLen);
        const std::string::size_type slash = dir.find_last_of("\\/");
        if (slash != std::string::npos)
            dir.resize(slash + 1);
        else
            dir.clear();
        candidates.push_back(dir + "data\\fonts\\vazir.ttf");
        candidates.push_back(dir + "fonts\\vazir.ttf");
    }
#endif
    candidates.push_back("data/fonts/vazir.ttf");
    candidates.push_back("fonts/vazir.ttf");
    candidates.push_back("vazir.ttf");
    candidates.push_back("../data/fonts/vazir.ttf");

    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        std::ifstream stream(candidates[i].c_str(), std::ios::in | std::ios::binary);
        if (!stream)
            continue;

        stream.seekg(0, std::ios::end);
        const std::streamoff length = stream.tellg();
        stream.seekg(0, std::ios::beg);
        if (length <= 0)
            continue;

        ttf->bytes.resize(static_cast<std::size_t>(length));
        stream.read(reinterpret_cast<char*>(&ttf->bytes[0]), length);
        if (!stream)
        {
            ttf->bytes.clear();
            continue;
        }

        const int offset = stbtt_GetFontOffsetForIndex(&ttf->bytes[0], 0);
        if (offset < 0 || !stbtt_InitFont(&ttf->font, &ttf->bytes[0], offset))
        {
            ttf->bytes.clear();
            continue;
        }

        ttf->path = candidates[i];
        ttf->ready = true;
        logDebugMessage(2, "IrFlags runtime TTF loaded: %s\n", ttf->path.c_str());
        return true;
    }

    logDebugMessage(1, "IrFlags could not load data/fonts/vazir.ttf; falling back to legacy bitmap fonts\n");
    return false;
}

void TextureFont::build(void)
{
    if (!ensureTtf())
        preLoadLists();
}

bool TextureFont::isBuilt() const
{
    if (ensureTtf())
        return true;
    return textureID != -1;
}

void TextureFont::preLoadLists()
{
    if (texture.size() < 1)
    {
        logDebugMessage(2,"Font %s does not have an associated texture name, not loading\n", texture.c_str());
        return;
    }
    TextureManager &tm = TextureManager::instance();
    std::string textureAndDir = "fonts/" + texture;
    textureID = tm.getTextureID(textureAndDir.c_str());
    if (textureID == -1)
    {
        logDebugMessage(2,"Font texture %s has invalid ID\n", texture.c_str());
        return;
    }
    logDebugMessage(4,"Font %s (face %s) has texture ID %d\n", texture.c_str(), faceName.c_str(), textureID);
    tm.setTextureFilter(textureID, OpenGLTexture::Nearest);

    for (int i = 0; i < numberOfCharacters; i++)
    {
        if (listIDs[i] != INVALID_GL_LIST_ID)
        {
            glDeleteLists(listIDs[i], 1);
            listIDs[i] = INVALID_GL_LIST_ID;
        }
        listIDs[i] = glGenLists(1);
        glNewList(listIDs[i], GL_COMPILE);
        {
            const float initiX = (float)fontMetrics[i].initialDist;
            const float fFontY = (float)(fontMetrics[i].endY - fontMetrics[i].startY);
            const float fFontX = (float)(fontMetrics[i].endX - fontMetrics[i].startX);
            const float startX = (float)fontMetrics[i].startX / (float)textureXSize;
            const float endX   = (float)fontMetrics[i].endX / (float)textureXSize;
            const float startY = (float)fontMetrics[i].startY / (float)textureYSize;
            const float endY   = (float)fontMetrics[i].endY / (float)textureYSize;
            glBegin(GL_TRIANGLE_STRIP);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(startX, 1.0f - startY); glVertex3f(initiX, fFontY, 0.0f);
            glTexCoord2f(startX, 1.0f - endY); glVertex3f(initiX, 0.0f, 0.0f);
            glTexCoord2f(endX, 1.0f - startY); glVertex3f(initiX + fFontX, fFontY, 0.0f);
            glTexCoord2f(endX, 1.0f - endY); glVertex3f(initiX + fFontX, 0.0f, 0.0f);
            glEnd();
            glTranslatef((float)(fontMetrics[i].fullWidth), 0.0f, 0.0f);
        }
        glEndList();
    }
    OpenGLGStateBuilder builder(gstate);
    builder.setTexture(textureID);
    builder.setBlending();
    builder.setAlphaFunc();
    gstate = builder.getState();
}

void TextureFont::clearTtfGlyphs()
{
    if (!ttf)
        return;
    for (std::map<std::uint32_t, TtfRuntime::Glyph>::iterator it = ttf->glyphs.begin();
         it != ttf->glyphs.end(); ++it)
    {
        if (it->second.texture != 0)
            glDeleteTextures(1, &it->second.texture);
    }
    ttf->glyphs.clear();
}

void TextureFont::free(void)
{
    clearTtfGlyphs();
    textureID = -1;
}

void TextureFont::filter(bool dofilter)
{
    if (ensureTtf())
        return;
    TextureManager &tm = TextureManager::instance();
    if (textureID >= 0)
        tm.setTextureFilter(textureID, dofilter ? OpenGLTexture::Max : OpenGLTexture::Nearest);
}

float TextureFont::getTtfLength(float scale, const char *str, int len) const
{
    if (!str || len <= 0 || !ensureTtf())
        return 0.0f;

    const std::vector<std::uint32_t> visual =
        IrFlagsPersian::prepareCodepoints(std::string(str, static_cast<std::size_t>(len)));
    const float pixelHeight = size > 0 ? static_cast<float>(size) : 16.0f;
    const float fontScale = stbtt_ScaleForPixelHeight(&ttf->font, pixelHeight);

    float lineWidth = 0.0f;
    float maxWidth = 0.0f;
    for (std::size_t i = 0; i < visual.size(); ++i)
    {
        const std::uint32_t cp = visual[i];
        if (cp == '\n')
        {
            maxWidth = std::max(maxWidth, lineWidth);
            lineWidth = 0.0f;
            continue;
        }

        int advance = 0;
        int bearing = 0;
        if (cp == '\t')
        {
            stbtt_GetCodepointHMetrics(&ttf->font, ' ', &advance, &bearing);
            lineWidth += 4.0f * advance * fontScale;
            continue;
        }

        stbtt_GetCodepointHMetrics(&ttf->font, static_cast<int>(cp), &advance, &bearing);
        lineWidth += advance * fontScale;
        if (i + 1 < visual.size() && visual[i + 1] != '\n')
        {
            lineWidth += stbtt_GetCodepointKernAdvance(&ttf->font,
                         static_cast<int>(cp), static_cast<int>(visual[i + 1])) * fontScale;
        }
    }
    maxWidth = std::max(maxWidth, lineWidth);
    return maxWidth * scale;
}

float TextureFont::getStrLength(float scale, const char *str, int len) const
{
    if (ensureTtf())
        return getTtfLength(scale, str, len);
    return ImageFont::getStrLength(scale, str, len);
}

float TextureFont::getStrLength(float scale, const std::string &str) const
{
    return getStrLength(scale, str.c_str(), static_cast<int>(str.size()));
}

void TextureFont::drawTtf(float scale, GLfloat color[4], const char *str, int len)
{
    if (!str || len <= 0 || !ensureTtf())
        return;

    const std::vector<std::uint32_t> visual =
        IrFlagsPersian::prepareCodepoints(std::string(str, static_cast<std::size_t>(len)));
    const float pixelHeight = size > 0 ? static_cast<float>(size) : 16.0f;
    const float fontScale = stbtt_ScaleForPixelHeight(&ttf->font, pixelHeight);

    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // FontManager uses a negative RGB triplet to mean "use the normal text color".
    // The legacy atlas renderer happened to inherit a usable color from its GState,
    // but a stand-alone TTF renderer cannot rely on the current OpenGL color.  Doing
    // so made all text transparent/black on some drivers.  Make that state explicit.
    if (color[0] >= 0.0f)
        glColor4fv(color);
    else
        glColor4f(1.0f, 1.0f, 1.0f, color[3] >= 0.0f ? color[3] : 1.0f);

    glPushMatrix();
    glScalef(scale, scale, 1.0f);

    float penX = 0.0f;
    float penY = 0.0f;

    for (std::size_t i = 0; i < visual.size(); ++i)
    {
        const std::uint32_t cp = visual[i];
        if (cp == '\n')
        {
            penX = 0.0f;
            penY -= pixelHeight * 1.35f;
            continue;
        }

        if (cp == '\t')
        {
            int advance = 0;
            int bearing = 0;
            stbtt_GetCodepointHMetrics(&ttf->font, ' ', &advance, &bearing);
            penX += 4.0f * advance * fontScale;
            continue;
        }

        std::map<std::uint32_t, TtfRuntime::Glyph>::iterator found = ttf->glyphs.find(cp);
        if (found == ttf->glyphs.end())
        {
            TtfRuntime::Glyph glyph;
            int advance = 0;
            int bearing = 0;
            stbtt_GetCodepointHMetrics(&ttf->font, static_cast<int>(cp), &advance, &bearing);
            glyph.advance = advance * fontScale;

            unsigned char *bitmap = stbtt_GetCodepointBitmap(&ttf->font, 0.0f, fontScale,
                                    static_cast<int>(cp), &glyph.width, &glyph.height,
                                    &glyph.xoff, &glyph.yoff);
            if (bitmap && glyph.width > 0 && glyph.height > 0)
            {
                // Use a conventional RGBA texture instead of GL_ALPHA.  The latter is
                // a legacy internal format and produced invisible glyphs on some modern
                // Windows OpenGL drivers.  White RGB lets GL_MODULATE apply UI colors;
                // the stb coverage bitmap becomes the real alpha channel.
                const std::size_t pixelCount = static_cast<std::size_t>(glyph.width) *
                                               static_cast<std::size_t>(glyph.height);
                std::vector<unsigned char> rgba(pixelCount * 4u, 255u);
                for (std::size_t px = 0; px < pixelCount; ++px)
                    rgba[(px * 4u) + 3u] = bitmap[px];

                glGenTextures(1, &glyph.texture);
                glBindTexture(GL_TEXTURE_2D, glyph.texture);
                GLint previousUnpackAlignment = 4;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph.width, glyph.height,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, &rgba[0]);
                glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
            }
            if (bitmap)
                stbtt_FreeBitmap(bitmap, NULL);
            found = ttf->glyphs.insert(std::make_pair(cp, glyph)).first;
        }

        const TtfRuntime::Glyph &glyph = found->second;
        if (glyph.texture != 0 && glyph.width > 0 && glyph.height > 0)
        {
            const float left = penX + static_cast<float>(glyph.xoff);
            const float right = left + static_cast<float>(glyph.width);
            const float top = penY - static_cast<float>(glyph.yoff);
            const float bottom = top - static_cast<float>(glyph.height);

            glBindTexture(GL_TEXTURE_2D, glyph.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(left, top, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(right, top, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(right, bottom, 0.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(left, bottom, 0.0f);
            glEnd();
        }

        penX += glyph.advance;
        if (i + 1 < visual.size() && visual[i + 1] != '\n')
        {
            penX += stbtt_GetCodepointKernAdvance(&ttf->font,
                    static_cast<int>(cp), static_cast<int>(visual[i + 1])) * fontScale;
        }
    }

    glPopMatrix();
    glPopAttrib();
    if (color[0] >= 0)
        glColor4f(1, 1, 1, 1);
}

void TextureFont::drawString(float scale, GLfloat color[4], const char *str, int len)
{
    if (!str || len <= 0)
        return;

    if (ensureTtf())
    {
        drawTtf(scale, color, str, len);
        return;
    }

    if (textureID == -1)
        preLoadLists();
    if (textureID == -1)
        return;

    gstate.setState();
    TextureManager &tm = TextureManager::instance();
    if (!tm.bind(textureID))
        return;
    if (color[0] >= 0)
        glColor4fv(color);

    glPushMatrix();
    glScalef(scale, scale, 1);
    for (int i = 0; i < len; i++)
    {
        const unsigned int glyph = static_cast<unsigned char>(str[i]);
        int charToUse = 32;
        if (glyph >= 32 && glyph < static_cast<unsigned int>(numberOfCharacters + 32))
            charToUse = static_cast<int>(glyph);
        charToUse -= 32;
        if (charToUse == 0)
            glTranslatef((float)(fontMetrics[charToUse].fullWidth), 0.0f, 0.0f);
        else
            glCallList(listIDs[charToUse]);
    }
    glPopMatrix();
    if (color[0] >= 0)
        glColor4f(1, 1, 1, 1);
}

// Local Variables: ***
// mode: C++ ***
// tab-width: 4 ***
// c-basic-offset: 4 ***
// indent-tabs-mode: nil ***
// End: ***
// ex: shiftwidth=4 tabstop=4
