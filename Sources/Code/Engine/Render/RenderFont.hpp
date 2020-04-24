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

    RenderFont();
    ~RenderFont();

    const Vec2i& getTexSize() const;
    int getTexId() const;
    bool createAtlas(unsigned int width, unsigned int height);
    void addGlyph(int ch, int shift, const RenderGlyph& glyphData, const void* buffer);
    const RenderGlyph* getGlyph(int ch) const;
    int getHeight() const;

private:

    std::unordered_map<int, RenderGlyph> glyphs;
    std::shared_ptr<RenderTexture> tex;
    int fontHeight;
};

#endif /* __RENDER_FONT_HPP__ */