#include "Nodes/TextNode.hpp"
#include "RenderTexture.hpp"
#include "Render/ETRenderManager.hpp"
#include "Logics/RenderAuxFunctions.hpp"

#include <cassert>

TextNode::TextNode() :
    color(255, 255, 255),
    fontHeight(24),
    alignAtCenter(true),
    doUpdate(true) {
}

TextNode::~TextNode() {
}

void TextNode::onInit() {
    setGeometry(PrimitiveGeometryType::Text_Vert_Chunk);
    setShader("text_solid_color");
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    ET_SendEventReturn(font, &ETRenderFontManager::ET_getDefaultFont);
}

void TextNode::setFontHeight(int newHeight) {
    fontHeight = newHeight;
}

void TextNode::setColor0(const ColorB& newColor) {
    color = newColor;
}

void TextNode::setText(const std::string& newText) {
    text = newText;
    doUpdate = true;
}

void TextNode::drawLines() {
    auto fontScale = fontHeight / static_cast<float>(font->getHeight());
    const auto scale = Vec2(tm.scale.x, tm.scale.y) * fontScale;

    AABB2D textBox;
    textBox.bot = Vec2(0.f);
    textBox.top = Vec2(textMetric.size.x * scale.x, textMetric.size.y * scale.y);

    Vec2 drawPt(tm.pt.x, tm.pt.y);
    if(alignAtCenter) {
        textBox.setCenter(drawPt);
    } else {
        textBox.bot += drawPt;
        textBox.top += drawPt;
    }

    drawPt = Vec2(textBox.bot.x, textBox.top.y);

    unsigned int vertShift = 0;
    std::vector<Vec4> vertChunk(geom->vertCount);

    for(size_t i = 0, sz = textMetric.lineMetrics.size(); i < sz; ++i) {
        const auto& lineMetric = textMetric.lineMetrics[i];
        if(i == 0) {
            drawPt.y -= textMetric.firstLineOffset * scale.y;
        } else {
            drawPt.y -= font->getHeight() * Render::TextNewLineOffset * scale.y;
        }
        drawPt.x = textBox.bot.x;
        vertShift = drawLine(drawPt, scale, vertShift, &vertChunk[0], lineMetric.startIdx, lineMetric.endIdx);
    }
    if(vertShift > 0) {
        geom->drawChunk(&vertChunk[0], vertShift);
    }
}

unsigned int TextNode::drawLine(const Vec2& pt, const Vec2& scale, unsigned int vertShift, Vec4* vertChunk, size_t startIdx, size_t endIdx) {
    Vec2 drawPt = pt;
    for(size_t i = startIdx; i < endIdx; ++i) {
        auto ch = text[i];
        assert(ch != '\n' && "Invalid new line character");
        auto glyph = font->getGlyph(ch);
        if(!glyph) {
            continue;
        }

        Vec2 glyphPt(0);
        glyphPt.x = drawPt.x;
        if(i > startIdx) {
            glyphPt.x += glyph->bearing.x * scale.x;
        }
        glyphPt.y = drawPt.y - (glyph->size.y - glyph->bearing.y) * scale.y;
        const float w = glyph->size.x * scale.x;
        const float h = glyph->size.y * scale.y;

        const auto& txBot = glyph->texCoords.bot;
        const auto& txTop = glyph->texCoords.top;

        // First tri
        vertChunk[vertShift + 0] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        vertChunk[vertShift + 1] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };
        vertChunk[vertShift + 2] = { glyphPt.x    , glyphPt.y    , txBot.x, txTop.y };
        // Second tri
        vertChunk[vertShift + 3] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        vertChunk[vertShift + 4] = { glyphPt.x + w, glyphPt.y + h, txTop.x, txBot.y };
        vertChunk[vertShift + 5] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };
        vertShift += 6;

        drawPt.x += glyph->advance.x * scale.x;
        drawPt.y += glyph->advance.y * scale.y;

        if(vertShift >= geom->vertCount) {
            if(vertShift == geom->vertCount) {
                geom->drawChunk(&vertChunk[0], vertShift);
                vertShift = 0;
            } else {
                geom->drawChunk(&vertChunk[0], vertShift - 6);
                for(int i = 0; i < 6; ++i) {
                    vertChunk[i] = vertChunk[vertShift - 6 + i];
                }
                vertShift = 6;
            }
        }
    }

    return vertShift;
}

void TextNode::onRender(RenderContext& ctx) {
    if(doUpdate) {
        doUpdate = false;
        textMetric = font->getTextMetric(text);
    }

    auto tex = font->getFontAtlas();

    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniform4f(UniformType::Color, color);

    drawLines();
}

bool TextNode::isVisible() const {
    if(text.empty()) {
        return false;
    }
    if(!font || !font->getFontAtlas()) {
        return false;
    }
    return Node::isVisible();
}

void TextNode::setAlignAtCenter(bool flag) {
    alignAtCenter = flag;
}