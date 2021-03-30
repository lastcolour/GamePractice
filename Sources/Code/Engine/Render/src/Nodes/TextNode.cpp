#include "Nodes/TextNode.hpp"
#include "RenderTexture.hpp"
#include "Render/ETRenderManager.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

TextNode::TextNode() :
    color(255, 255, 255),
    fontHeight(24.f),
    alignAtCenter(true) {
}

TextNode::~TextNode() {
}

void TextNode::onInit() {
    setGeometry(PrimitiveGeometryType::Text);
    setShader("text_solid_color");
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    ET_SendEventReturn(font, &ETRenderFontManager::ET_getDefaultFont);
}

void TextNode::setFontHeight(float newFontHeight) {
    fontHeight = newFontHeight;
    setModelMatDirty();
}

void TextNode::setColor0(const ColorB& newColor) {
    color = newColor;
}

void TextNode::setText(std::string&& newText) {
    text = std::move(newText);
    setModelMatDirty();
}

void TextNode::setText(const std::string& newText) {
    text = newText;
    setModelMatDirty();
}

Mat4 TextNode::calcModelMat(const Transform& newTm) {
    textMetric = font->getTextMetric(text);

    Transform resTm = newTm;

    auto scale = fontHeight / static_cast<float>(font->getHeight());
    resTm.scale.x *= scale;
    resTm.scale.y *= scale;

    return resTm.toMat4();
}

void TextNode::drawLines() {
    unsigned int vertShift = 0;
    vertData.reserve(text.size() * 6);

    float xStart = 0.f;
    float yStart = textMetric.size.y / 2.f;
    if(alignAtCenter) {
        xStart = -textMetric.size.x / 2.f;
    }

    Vec2 drawPt(0.f, yStart);
    for(size_t i = 0, sz = textMetric.lineMetrics.size(); i < sz; ++i) {
        const auto& lineMetric = textMetric.lineMetrics[i];
        if(i == 0) {
            drawPt.y -= textMetric.firstLineOffset;
        } else {
            drawPt.y -= font->getHeight() * RenderUtils::TextNewLineOffset;
        }
        drawPt.x = xStart;
        vertShift = drawLine(drawPt, vertShift, lineMetric.startIdx, lineMetric.endIdx);
    }

    if(vertShift > 0) {
        geom->drawText(&vertData[0], vertShift);
    }
}

unsigned int TextNode::drawLine(const Vec2& pt, unsigned int vertShift, size_t startIdx, size_t endIdx) {
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
            glyphPt.x += glyph->bearing.x;
        }
        glyphPt.y = drawPt.y - (glyph->size.y - glyph->bearing.y);
        const float w = static_cast<float>(glyph->size.x);
        const float h = static_cast<float>(glyph->size.y);

        const auto& txBot = glyph->texCoords.bot;
        const auto& txTop = glyph->texCoords.top;

        // First tri
        vertData[vertShift + 0] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        vertData[vertShift + 1] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };
        vertData[vertShift + 2] = { glyphPt.x    , glyphPt.y    , txBot.x, txTop.y };
        // Second tri
        vertData[vertShift + 3] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        vertData[vertShift + 4] = { glyphPt.x + w, glyphPt.y + h, txTop.x, txBot.y };
        vertData[vertShift + 5] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };

        vertShift += 6;

        drawPt.x += glyph->advance.x;
        drawPt.y += glyph->advance.y;
    }

    return vertShift;
}

void TextNode::onRender(RenderContext& ctx) {
    auto tex = font->getFontAtlas();

    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    shader->setUniform4f(UniformType::Color, color);

    drawLines();
}

bool TextNode::canRender() const {
    if(text.empty()) {
        return false;
    }
    if(!font || !font->getFontAtlas()) {
        return false;
    }
    return Node::canRender();
}

void TextNode::setAlignAtCenter(bool flag) {
    alignAtCenter = flag;
}