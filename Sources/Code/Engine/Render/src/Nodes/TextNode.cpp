#include "Nodes/TextNode.hpp"
#include "RenderTexture.hpp"
#include "Render/ETRenderManager.hpp"

namespace {

const float NEXT_LINE_OFFSET = 1.2f;

} // namespace

TextNode::TextNode() :
    color(255, 255, 255),
    fontHeight(24) {
}

TextNode::~TextNode() {
}

void TextNode::onInit() {
    setGeometry(PrimitiveGeometryType::Text_Vert_Chunk);
    setMaterial("text_solid_color");
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createDefaultFont);
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

void TextNode::onRender(RenderContext& ctx) {
    mat->setTexture2D("tex", font->getFontAtlas()->texId);
    mat->setUniformMat4("MVP", ctx.proj2dMat);
    mat->setUniform4f("color", color);

    auto fontScale = fontHeight / static_cast<float>(font->getHeight());
    const auto scale = Vec2(tm.scale.x, tm.scale.y) * fontScale;

    auto textSize = font->getTextSize(text);

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(textSize.x * scale.x, textSize.y * scale.y);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));

    Vec2 pt = Vec2(aabb.bot.x, aabb.top.y);
    pt.y -= font->getHeight() * scale.y;

    std::vector<Vec4> vertChunk(geom->vertCount);
    unsigned int vertShift = 0;
    for(auto it = text.begin();;) {
        if(it != text.end()) {
            auto ch = *it;
            if(ch == '\n') {
                pt.y -= font->getHeight() * NEXT_LINE_OFFSET * scale.y;
                pt.x = aabb.bot.x;
            } else if(auto glyph = font->getGlyph(ch)) {
                Vec2 glyphPt(0);
                glyphPt.x = pt.x + glyph->bearing.x * scale.x;
                glyphPt.y = pt.y - (glyph->size.y - glyph->bearing.y) * scale.y;
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

                pt.x += glyph->advance.x * scale.x;
                pt.y += glyph->advance.y * scale.y;
            }
        }

        if(vertShift >= geom->vertCount) {
            if(vertShift == geom->vertCount) {
                ++it;
                geom->drawChunk(&vertChunk[0], vertShift);
            } else {
                geom->drawChunk(&vertChunk[0], vertShift - 6);
            }
            vertShift = 0;
        } else if(it == text.end()) {
            geom->drawChunk(&vertChunk[0], vertShift);
            break;
        } else {
            ++it;
        }
    }
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