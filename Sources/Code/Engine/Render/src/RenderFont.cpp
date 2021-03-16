#include "RenderFont.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"
#include "Render/ETRenderManager.hpp"
#include "Logics/RenderAuxFunctions.hpp"

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

Vec2i RenderFont::getTextSize(const std::string& text) const {
    auto textMetric = getTextMetric(text);
    return textMetric.size;
}

TextMetric RenderFont::getTextMetric(const std::string& text) const {
    TextMetric res;
    res.firstLineOffset = 0;
    res.size = Vec2i(0);

    if(text.empty()) {
        return res;
    }

    TextMetric::LineMetric currLineMetric;
    currLineMetric.startIdx = 0;
    currLineMetric.endIdx = 0;
    currLineMetric.lineLen = 0;

    int currLine = 0;
    int currCharInLine = 0;
    int prevLineOffset = 0;
    const RenderGlyph* prevGlyph = nullptr;

    for(size_t i = 0, sz = text.size(); i < sz; ++i) {
        auto ch = text[i];
        if(ch == '\n') {
            if(prevGlyph) {
                currLineMetric.lineLen -= prevGlyph->advance.x - (prevGlyph->bearing.x + prevGlyph->size.x);
            }

            currLineMetric.endIdx = static_cast<int>(i);
            res.lineMetrics.push_back(currLineMetric);

            ++currLine;
            currCharInLine = 0;
            prevLineOffset = 0;
            currLineMetric.lineLen = 0;
            currLineMetric.startIdx = static_cast<int>(i + 1);
            currLineMetric.endIdx = 0;
        } else {
            auto glyph = getGlyph(ch);
            prevGlyph = glyph;
            if(!glyph) {
                continue;
            }
            if(currLine == 0) {
                res.firstLineOffset = std::max(res.firstLineOffset, glyph->bearing.y);
            }
            if(currCharInLine == 0) {
                currLineMetric.lineLen += glyph->advance.x - glyph->bearing.x;
            } else {
                currLineMetric.lineLen += glyph->advance.x;
            }
            prevLineOffset = std::min(prevLineOffset, glyph->bearing.y - glyph->size.y);
            ++currCharInLine;
        }
    }

    if(prevGlyph) {
        currLineMetric.lineLen -= prevGlyph->advance.x - (prevGlyph->bearing.x + prevGlyph->size.x);
    }
    currLineMetric.endIdx = static_cast<int>(text.size());
    res.lineMetrics.push_back(currLineMetric);

    res.size.y = fontHeight;
    res.size.x = res.lineMetrics[0].lineLen;
    for(size_t i = 1, sz = res.lineMetrics.size(); i < sz; ++i) {
        auto& lineMetric = res.lineMetrics[i];
        res.size.x = std::max(res.size.x, lineMetric.lineLen);
        res.size.y += static_cast<int>(fontHeight * Render::TextNewLineOffset);
    }

    res.size.y -= fontHeight - res.firstLineOffset;
    res.size.y -= prevLineOffset;

    return res;
}