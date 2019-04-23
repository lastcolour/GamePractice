#ifndef __RENDER_FONT_HPP__
#define __RENDER_FONT_HPP__

#include "Math/AABB.hpp"

#include <unordered_map>

class RenderGlyph {
public:
    Vec2i advance;
    Vec2i size;
    Vec2i bearing;
    AABB2D texCoords;
};

class RenderFont {
public:

    RenderFont();
    ~RenderFont();

    const Vec2i& getTexSize() const;
    int getTexId() const;
    bool createAtlas(unsigned int width, unsigned int height);
    void addGlyph(int ch, const RenderGlyph& glyphData, const void* buffer);
    const RenderGlyph* getGlyph(int ch) const;

private:

    std::unordered_map<int, RenderGlyph> glyphs;
    Vec2i texSize;
    unsigned int textureId;
};

#endif /* __RENDER_FONT_HPP__ */