#ifndef __RENDER_FONT_HPP__
#define __RENDER_FONT_HPP__

#include "Math/AABB.hpp"

#include <unordered_map>
#include <memory>

class RenderTexture;

class RenderGlyph {
public:
    Vec2i advance;
    Vec2i size;
    Vec2i bearing;
    AABB2D texCoords;
};

class RenderFont {
public:

    RenderFont(int fontMaxHeight);
    ~RenderFont();

    void setFontAtlas(std::shared_ptr<RenderTexture>& newFontAtlas);
    const RenderTexture* getFontAtlas() const;
    void addGlyph(int ch, int shift, const RenderGlyph& glyphData);
    const RenderGlyph* getGlyph(int ch) const;
    int getHeight() const;
    Vec2i getTextSize(const std::string& text) const;
    Vec2i getTextSize(const char* cStr, size_t len) const;

private:

    std::unordered_map<int, RenderGlyph> glyphs;
    std::shared_ptr<RenderTexture> fontAtlas;
    int fontHeight;
};

#endif /* __RENDER_FONT_HPP__ */