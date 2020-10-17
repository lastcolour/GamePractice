#include "RenderFont.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"
#include "Render/ETRenderManager.hpp"
#include "Logics/RenderAuxFunctions.hpp"

#include <algorithm>
#include <cassert>

RenderFont::RenderFont(int fontMaxHeight) :
    fontHeight(fontMaxHeight) {
}

RenderFont::~RenderFont() {
}

void RenderFont::setFontAtlas(std::shared_ptr<RenderTexture>& newFontAtlas) {
    fontAtlas = newFontAtlas;
}

const RenderTexture* RenderFont::getFontAtlas() const {
    return fontAtlas.get();
}

void RenderFont::addGlyph(int ch, int shift, const RenderGlyph& glyphData) {
    glyphs[ch] = glyphData;
}

int RenderFont::getHeight() const {
    return fontHeight;
}

const RenderGlyph* RenderFont::getGlyph(int ch) const {
    auto it = glyphs.find(ch);
    if(it != glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}

Vec2i RenderFont::getTextSize(const char* cStr, size_t len) const {
    Vec2i res(0);
    if(len == 0) {
        return res;
    }

    int currLineIdx = 0;
    int maxLineLen = 0;
    int currLineMaxY = 0;
    int currLineMinY = 0;
    int currLineLen = 0;

    res.y = fontHeight;

    for(size_t i = 0; i < len; ++i) {
        auto ch = cStr[i];
        if(ch == '\n') {
            maxLineLen = std::max(maxLineLen, currLineLen);
            currLineLen = 0;

            res.y += static_cast<int>(fontHeight * Render::TextNewLineOffset);
            if(currLineIdx == 0) {
                res.y += (currLineMaxY - currLineMinY) - fontHeight;
            }

            ++currLineIdx;
            currLineMaxY = 0;
        } else {
            auto glyph = getGlyph(ch);
            if(!glyph) {
                continue;
            }
            if(i + 1u < len) {
                currLineLen += glyph->advance.x;
            } else {
                if(ch == ' ') {
                    currLineLen += glyph->advance.x;
                } else {
                    currLineLen += glyph->size.x;
                }
            }
            currLineMaxY = std::max(currLineMaxY, glyph->bearing.y);
            currLineMinY = std::min(currLineMinY, glyph->bearing.y - glyph->size.y);
        }
    }

    res.x = std::max(maxLineLen, currLineLen);
    res.y += (currLineMaxY - currLineMinY) - fontHeight;

    return res;
}

Vec2i RenderFont::getTextSize(const std::string& text) const {
    return getTextSize(text.c_str(), text.size());
}