#include "Nodes/TextNode.hpp"
#include "RenderTexture.hpp"
#include "Render/ETRenderManager.hpp"
#include "Logics/RenderAuxFunctions.hpp"

#include <cassert>

namespace {

std::vector<std::pair<size_t, size_t>> getLinesStartEndIdx(const std::string& text) {
    std::vector<std::pair<size_t, size_t>> lines;
    size_t prevLineEnd = 0;
    for(size_t i = 0, sz = text.size(); i < sz; ++i) {
        auto ch = text[i];
        if(ch == '\n') {
            lines.push_back({prevLineEnd, i});
            prevLineEnd = i + 1;
        }
    }
    if(prevLineEnd < text.size()) {
        lines.push_back({prevLineEnd, text.size()});
    }
    return lines;
}

} // namespace

TextNode::TextNode() :
    color(255, 255, 255),
    fontHeight(24),
    alignAtCenter(true) {
}

TextNode::~TextNode() {
}

void TextNode::onInit() {
    setGeometry(PrimitiveGeometryType::Text_Vert_Chunk);
    setMaterial("text_solid_color");
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    ET_SendEventReturn(font, &ETRenderFontManager::ET_getDefaultFont);
}

void TextNode::ET_setFontHeight(int newHeight) {
    fontHeight = newHeight;
}

void TextNode::ET_setColor0(const ColorB& newColor) {
    color = newColor;
}

void TextNode::ET_setText(const std::string& newText) {
    text = newText;
}

void TextNode::drawLines() {
    auto fontScale = fontHeight / static_cast<float>(font->getHeight());
    const auto scale = Vec2(tm.scale.x, tm.scale.y) * fontScale;

    auto textSize = font->getTextSize(text);

    AABB2D textBox;
    textBox.bot = Vec2(0.f);
    textBox.top = Vec2(textSize.x * scale.x, textSize.y * scale.y);

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

    auto lineStartEndIdx = getLinesStartEndIdx(text);

    for(size_t i = 0, sz = lineStartEndIdx.size(); i < sz; ++i) {
        if(i == 0) {
            Vec2i firstLineSize = font->getTextSize(text.c_str(), lineStartEndIdx[i].second);
            drawPt.y -= (font->getHeight() * Render::TextNewLineOffset - (font->getHeight() - firstLineSize.y)) * scale.y;
        } else {
            drawPt.y -= font->getHeight() * Render::TextNewLineOffset * scale.y;
        }
        drawPt.x = textBox.bot.x;
        vertShift = drawLine(drawPt, scale, vertShift, &vertChunk[0], lineStartEndIdx[i].first, lineStartEndIdx[i].second);
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
        glyphPt.x = drawPt.x + glyph->bearing.x * scale.x;
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
    mat->setTexture2D("tex", font->getFontAtlas()->texId);
    mat->setUniformMat4("MVP", ctx.proj2dMat);
    mat->setUniform4f("color", color);

    drawLines();
}

bool TextNode::isVisible() const {
    if(text.empty()) {
        return false;
    }
    if(!font->getFontAtlas()) {
        return false;
    }
    return Node::isVisible();
}

void TextNode::setAlignAtCenter(bool flag) {
    alignAtCenter = flag;
}