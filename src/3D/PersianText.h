#ifndef IRFLAGS_PERSIAN_TEXT_H
#define IRFLAGS_PERSIAN_TEXT_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace IrFlagsPersian
{
struct GlyphDef
{
    std::uint32_t base;
    std::uint32_t isolated;
    std::uint32_t finalForm;
    std::uint32_t initial;
    std::uint32_t medial;
    unsigned char forms;
    bool joinsPrevious;
    bool joinsNext;
};

static const GlyphDef glyphs[] =
{
    {0x0621,0xFE80,0,0,0,1,false,false},
    {0x0622,0xFE81,0xFE82,0,0,2,true,false},
    {0x0623,0xFE83,0xFE84,0,0,2,true,false},
    {0x0624,0xFE85,0xFE86,0,0,2,true,false},
    {0x0625,0xFE87,0xFE88,0,0,2,true,false},
    {0x0626,0xFE89,0xFE8A,0xFE8B,0xFE8C,4,true,true},
    {0x0627,0xFE8D,0xFE8E,0,0,2,true,false},
    {0x0628,0xFE8F,0xFE90,0xFE91,0xFE92,4,true,true},
    {0x067E,0xFB56,0xFB57,0xFB58,0xFB59,4,true,true},
    {0x062A,0xFE95,0xFE96,0xFE97,0xFE98,4,true,true},
    {0x062B,0xFE99,0xFE9A,0xFE9B,0xFE9C,4,true,true},
    {0x062C,0xFE9D,0xFE9E,0xFE9F,0xFEA0,4,true,true},
    {0x0686,0xFB7A,0xFB7B,0xFB7C,0xFB7D,4,true,true},
    {0x062D,0xFEA1,0xFEA2,0xFEA3,0xFEA4,4,true,true},
    {0x062E,0xFEA5,0xFEA6,0xFEA7,0xFEA8,4,true,true},
    {0x062F,0xFEA9,0xFEAA,0,0,2,true,false},
    {0x0630,0xFEAB,0xFEAC,0,0,2,true,false},
    {0x0631,0xFEAD,0xFEAE,0,0,2,true,false},
    {0x0632,0xFEAF,0xFEB0,0,0,2,true,false},
    {0x0698,0xFB8A,0xFB8B,0,0,2,true,false},
    {0x0633,0xFEB1,0xFEB2,0xFEB3,0xFEB4,4,true,true},
    {0x0634,0xFEB5,0xFEB6,0xFEB7,0xFEB8,4,true,true},
    {0x0635,0xFEB9,0xFEBA,0xFEBB,0xFEBC,4,true,true},
    {0x0636,0xFEBD,0xFEBE,0xFEBF,0xFEC0,4,true,true},
    {0x0637,0xFEC1,0xFEC2,0xFEC3,0xFEC4,4,true,true},
    {0x0638,0xFEC5,0xFEC6,0xFEC7,0xFEC8,4,true,true},
    {0x0639,0xFEC9,0xFECA,0xFECB,0xFECC,4,true,true},
    {0x063A,0xFECD,0xFECE,0xFECF,0xFED0,4,true,true},
    {0x0641,0xFED1,0xFED2,0xFED3,0xFED4,4,true,true},
    {0x0642,0xFED5,0xFED6,0xFED7,0xFED8,4,true,true},
    {0x06A9,0xFB8E,0xFB8F,0xFB90,0xFB91,4,true,true},
    {0x06AF,0xFB92,0xFB93,0xFB94,0xFB95,4,true,true},
    {0x0644,0xFEDD,0xFEDE,0xFEDF,0xFEE0,4,true,true},
    {0x0645,0xFEE1,0xFEE2,0xFEE3,0xFEE4,4,true,true},
    {0x0646,0xFEE5,0xFEE6,0xFEE7,0xFEE8,4,true,true},
    {0x0647,0xFEE9,0xFEEA,0xFEEB,0xFEEC,4,true,true},
    {0x0648,0xFEED,0xFEEE,0,0,2,true,false},
    {0x06CC,0xFBFC,0xFBFD,0xFBFE,0xFBFF,4,true,true},
    {0x0649,0xFEEF,0xFEF0,0,0,2,true,false},
    {0x0629,0xFE93,0xFE94,0,0,2,true,false}
};

inline std::uint32_t normalize(std::uint32_t cp)
{
    if (cp == 0x064A || cp == 0x0649)
        return 0x06CC;
    if (cp == 0x0643)
        return 0x06A9;
    return cp;
}

inline const GlyphDef* findGlyph(std::uint32_t cp)
{
    cp = normalize(cp);
    for (std::size_t i = 0; i < sizeof(glyphs)/sizeof(glyphs[0]); ++i)
        if (glyphs[i].base == cp)
            return &glyphs[i];
    return NULL;
}

inline bool isMark(std::uint32_t cp)
{
    return (cp >= 0x064B && cp <= 0x065F) || cp == 0x0670 ||
           (cp >= 0x06D6 && cp <= 0x06ED);
}

inline std::vector<std::uint32_t> decodeUtf8(const std::string& input)
{
    std::vector<std::uint32_t> out;
    for (std::size_t i = 0; i < input.size();)
    {
        const unsigned char c = static_cast<unsigned char>(input[i]);
        if (c < 0x80)
        {
            out.push_back(c);
            ++i;
        }
        else if ((c & 0xE0) == 0xC0 && i + 1 < input.size())
        {
            out.push_back(((c & 0x1F) << 6) |
                          (static_cast<unsigned char>(input[i+1]) & 0x3F));
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0 && i + 2 < input.size())
        {
            out.push_back(((c & 0x0F) << 12) |
                          ((static_cast<unsigned char>(input[i+1]) & 0x3F) << 6) |
                          (static_cast<unsigned char>(input[i+2]) & 0x3F));
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0 && i + 3 < input.size())
        {
            out.push_back(((c & 0x07) << 18) |
                          ((static_cast<unsigned char>(input[i+1]) & 0x3F) << 12) |
                          ((static_cast<unsigned char>(input[i+2]) & 0x3F) << 6) |
                          (static_cast<unsigned char>(input[i+3]) & 0x3F));
            i += 4;
        }
        else
        {
            out.push_back(0xFFFD);
            ++i;
        }
    }
    return out;
}

inline void appendUtf8(std::string& out, std::uint32_t cp)
{
    if (cp <= 0x7F)
        out.push_back(static_cast<char>(cp));
    else if (cp <= 0x7FF)
    {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
    else if (cp <= 0xFFFF)
    {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
    else
    {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

inline bool joinAcross(const std::vector<std::uint32_t>& cps, std::size_t left,
                       std::size_t right)
{
    if (left >= cps.size() || right >= cps.size())
        return false;
    const GlyphDef* a = findGlyph(cps[left]);
    const GlyphDef* b = findGlyph(cps[right]);
    return a && b && a->joinsNext && b->joinsPrevious;
}

inline std::uint32_t shapedCodepoint(const GlyphDef* glyph, bool prev, bool next)
{
    if (!glyph)
        return 0xFFFD;
    if (glyph->forms == 1)
        return glyph->isolated;
    if (glyph->forms == 2)
        return prev && glyph->finalForm ? glyph->finalForm : glyph->isolated;
    if (prev && next && glyph->medial)
        return glyph->medial;
    if (prev && glyph->finalForm)
        return glyph->finalForm;
    if (next && glyph->initial)
        return glyph->initial;
    return glyph->isolated;
}

inline std::uint32_t mirror(std::uint32_t cp)
{
    if (cp == '(') return ')';
    if (cp == ')') return '(';
    if (cp == '[') return ']';
    if (cp == ']') return '[';
    if (cp == '{') return '}';
    if (cp == '}') return '{';
    if (cp == '<') return '>';
    if (cp == '>') return '<';
    return cp;
}

inline std::vector<std::uint32_t> prepareLineCodepoints(const std::string& input)
{
    const std::vector<std::uint32_t> decoded = decodeUtf8(input);
    std::vector<std::uint32_t> cps;
    cps.reserve(decoded.size());
    bool hasPersian = false;

    for (std::size_t i = 0; i < decoded.size(); ++i)
    {
        std::uint32_t cp = normalize(decoded[i]);
        if (isMark(cp))
            continue;
        if (cp == 0x200D)
            cp = 0x200C;
        if (findGlyph(cp))
            hasPersian = true;
        cps.push_back(cp);
    }

    if (!hasPersian)
        return cps;

    std::vector<std::vector<std::uint32_t> > units;
    for (std::size_t i = 0; i < cps.size();)
    {
        const GlyphDef* g = findGlyph(cps[i]);
        if (g)
        {
            bool prev = false;
            bool next = false;
            if (i > 0 && cps[i-1] != 0x200C)
                prev = joinAcross(cps, i-1, i);
            if (i + 1 < cps.size() && cps[i+1] != 0x200C)
                next = joinAcross(cps, i, i+1);
            std::vector<std::uint32_t> unit;
            unit.push_back(shapedCodepoint(g, prev, next));
            units.push_back(unit);
            ++i;
            continue;
        }

        if (cps[i] == 0x200C)
        {
            ++i;
            continue;
        }

        if (cps[i] == ' ' || cps[i] == '\t')
        {
            std::vector<std::uint32_t> unit;
            unit.push_back(cps[i]);
            units.push_back(unit);
            ++i;
            continue;
        }

        std::vector<std::uint32_t> ltr;
        while (i < cps.size() && !findGlyph(cps[i]) && cps[i] != 0x200C &&
               cps[i] != ' ' && cps[i] != '\t')
        {
            ltr.push_back(cps[i]);
            ++i;
        }
        if (!ltr.empty())
            units.push_back(ltr);
    }

    std::reverse(units.begin(), units.end());
    std::vector<std::uint32_t> result;
    for (std::size_t i = 0; i < units.size(); ++i)
    {
        for (std::size_t j = 0; j < units[i].size(); ++j)
        {
            std::uint32_t cp = units[i][j];
            if (units[i].size() == 1)
                cp = mirror(cp);
            result.push_back(cp);
        }
    }
    return result;
}

inline std::vector<std::uint32_t> prepareCodepoints(const std::string& input)
{
    std::vector<std::uint32_t> result;
    std::size_t begin = 0;
    while (begin <= input.size())
    {
        const std::size_t end = input.find('\n', begin);
        const std::string line = input.substr(begin,
            end == std::string::npos ? std::string::npos : end - begin);
        const std::vector<std::uint32_t> shaped = prepareLineCodepoints(line);
        result.insert(result.end(), shaped.begin(), shaped.end());
        if (end == std::string::npos)
            break;
        result.push_back('\n');
        begin = end + 1;
    }
    return result;
}

inline std::string prepare(const std::string& input)
{
    const std::vector<std::uint32_t> cps = prepareCodepoints(input);
    std::string result;
    for (std::size_t i = 0; i < cps.size(); ++i)
        appendUtf8(result, cps[i]);
    return result;
}
}

#endif
