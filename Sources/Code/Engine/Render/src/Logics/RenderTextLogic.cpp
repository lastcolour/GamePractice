#include "Logics/RenderTextLogic.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderContext.hpp"
#include "RenderMaterial.hpp"
#include "RenderFont.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platforms/OpenGL.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <algorithm>
#include <cassert>

namespace {

const float NEXT_LINE_OFFSET = 1.2f;

} // namespace

RenderTextLogic::RenderTextLogic() :
    color(255, 255, 255),
    fontScale(1.f) {
}

RenderTextLogic::~RenderTextLogic() {
}

void RenderTextLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderTextLogic>("RenderText")) {
        classInfo->addField("color", &RenderTextLogic::color);
        classInfo->addField("fontScale", &RenderTextLogic::fontScale);
        classInfo->addField("text", &RenderTextLogic::text);
    }
}

void RenderTextLogic::ET_setFont(const char* fontName) {
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createFont, fontName);
}

bool RenderTextLogic::init() {
    ET_setGeometry(PrimitiveGeometryType::Text_Vert_Chunk);
    if(!geom) {
        return false;
    }
    ET_setMaterial("text_solid_color");
    if(!mat) {
        return false;
    }

    ET_SendEventReturn(font, &ETRenderFontManager::ET_createDefaultFont);
    if(!font) {
        return false;
    }

    RenderNode::init();

    ETNode<ETRenderTextLogic>::connect(getEntityId());
    return true;
}

void RenderTextLogic::onRender(RenderContext& renderCtx) {
    renderCtx.setBlending(RenderBlendingType::SRC_MINUS_ALPHA);
    mat->bind();
    mat->setTexture2D("tex", font->getTexId());
    mat->setUniformMat4("MVP", renderCtx.proj2dMat);
    mat->setUniform4f("color", color);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(textSize.x * tm.scale.x, textSize.y * tm.scale.y);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));

    const auto scale = Vec2(tm.scale.x, tm.scale.y) * fontScale;
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
    mat->unbind();
    renderCtx.setBlending(RenderBlendingType::NONE);
}

void RenderTextLogic::calcTextSize() {
    Vec2 pt(0.f);
    pt.y = text.empty() ? 0.f : static_cast<float>(font->getHeight());
    float currentLineX = 0.f;
    for(size_t i = 0u, sz = text.size(); i < sz; ++i) {
        auto ch = text[i];
        if(auto glyph = font->getGlyph(ch)) {
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
            pt.y += font->getHeight() * NEXT_LINE_OFFSET;
            pt.x = std::max(pt.x, currentLineX);
            currentLineX = 0.f;
        }
    }
    pt.x = std::max(pt.x, currentLineX);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    textSize = pt * fontScale;
}

void RenderTextLogic::ET_setFontSize(int fontSize) {
    auto heigth = font->getHeight();
    fontScale = fontSize / static_cast<float>(heigth);
    calcTextSize();
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(textSize.x * tm.scale.x, textSize.y * tm.scale.y);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));

    return aabb;
}

void RenderTextLogic::ET_setColor(const ColorB& col) {
    color = col;
}

void RenderTextLogic::ET_setText(const char* str) {
    text = str;
    calcTextSize();
    if(!text.empty()) {
        ET_show();
    } else {
        ET_hide();
    }
}