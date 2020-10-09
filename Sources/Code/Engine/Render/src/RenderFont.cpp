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

Vec2i RenderFont::getTextSize(const std::string& text) const {
    Vec2i pt(0);
    pt.y = text.empty() ? 0 : fontHeight;
    int currentLineX = 0;
    for(size_t i = 0u, sz = text.size(); i < sz; ++i) {
        auto ch = text[i];
        if(auto glyph = getGlyph(ch)) {
            if(i + 1u < sz) {
                currentLineX += glyph->advance.x;
            } else {
                if(ch == ' ') {
                    currentLineX += glyph->advance.x;
                } else {
                    currentLineX += glyph->size.x;
                }
            }
        } else if(ch == '\n') {
            pt.y += static_cast<int>(fontHeight * Render::TextNewLineOffset);
            pt.x = std::max(pt.x, currentLineX);
            currentLineX = 0;
        }
    }
    pt.x = std::max(pt.x, currentLineX);
    return pt;
}