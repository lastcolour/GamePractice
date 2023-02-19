#ifndef __RENDER_FONT_HPP__
#define __RENDER_FONT_HPP__

#include "Math/AABB.hpp"

class RenderTexture;

struct RenderGlyph {
    Vec2i advance;
    Vec2i size;
    Vec2i bearing;
    AABB2D texCoords;
};

struct TextMetric {
public:

    struct LineMetric {
        int startIdx;
        int endIdx;
        int lineLen;
    };

public:

    TextMetric();
    void reset();

public:

    std::vector<LineMetric> lineMetrics;
    Vec2i size;
    int firstLineOffset;
};

class RenderFont {
public:

    RenderFont(int fontMaxHeight);
    ~RenderFont();

    void setFontAtlas(std::shared_ptr<RenderTexture>& newFontAtlas);
    RenderTexture* getFontAtlas();
    const RenderTexture* getFontAtlas() const;
    void addGlyph(int ch, int shift, const RenderGlyph& glyphData);
    const RenderGlyph* getGlyph(int ch) const;
    int getHeight() const;
    Vec2i getTextSize(const std::string& text) const;
    TextMetric getTextMetric(const std::string& text) const;

private:

    std::unordered_map<int, RenderGlyph> glyphs;
    std::shared_ptr<RenderTexture> fontAtlas;
    int fontHeight;
};

#endif /* __RENDER_FONT_HPP__ */