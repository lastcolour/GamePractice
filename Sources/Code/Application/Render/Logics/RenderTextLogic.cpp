#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderFont.hpp"
#include "Render/RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>
#include <cassert>

RenderTextLogic::RenderTextLogic() :
    color(255, 255, 255) {
}

RenderTextLogic::~RenderTextLogic() {
}

bool RenderTextLogic::serialize(const JSONNode& node) {
    std::string matName;
    node.value("mat", matName);
    ET_setMaterial(matName.c_str());
    return true;
}

bool RenderTextLogic::init() {
    if(!mat) {
        return false;
    }
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createDefaultFont);
    if(!font) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, "text_chunk");
    if(!geom) {
        return false;
    }

    assert(geom->vertCount >= 6 && "Invalid chunk vertext count");
    assert(geom->vertType == VertexType::Vector4 && "Invalid vertext type");

    ETNode<ETRenderTextLogic>::connect(getEntityId());
    return true;
}

void RenderTextLogic::ET_onRender(const RenderContext& renderCtx) {
    mat->bind();
    mat->setTexture2D("tex", font->getTexId());
    mat->setUniformMat4("MVP", renderCtx.proj2dMat);
    mat->setUniform4f("color", color);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));
    const auto scale = Vec2(tm.scale.x, tm.scale.y);
    Vec2 pt = Vec2(aabb.bot.x, aabb.bot.y);

    std::vector<Vec4> vertChunk(geom->vertCount);
    unsigned int vertShift = 0;
    for(auto it = text.begin();;) {
        Vec2 glyphPt(0);
        if (it != text.end()) {
            if(auto glyph = font->getGlyph(*it)) {
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
            if (vertShift == geom->vertCount) {
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
}

void RenderTextLogic::calcTextAABB() {
    Vec2 pt(0.f);
    pt.y = text.empty() ? 0.f : static_cast<float>(font->getHeight());
    for(size_t i = 0u, sz = text.size(); i < sz; ++i) {
        auto ch = text[i];
        if(auto glyph = font->getGlyph(ch)) {
            if (i + 1u < sz) {
                pt.x += glyph->advance.x;
            } else {
                if (ch == ' ') {
                    pt.x += glyph->advance.x;
                } else {
                    pt.x += glyph->size.x;
                }
            }
        }
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);

    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(pt.x * tm.scale.x, pt.y * tm.scale.y);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));
}

void RenderTextLogic::ET_setFontSize(float newFontSize) {
    fontSize = newFontSize;
}

const AABB2D& RenderTextLogic::ET_getTextAABB() const {
    return aabb;
}

void RenderTextLogic::ET_setMaterial(const char* matName) {
    ET_SendEventReturn(mat, &ETRenderMaterialManager::ET_createMaterial, matName);
}

void RenderTextLogic::ET_setColor(const ColorB& col) {
    color = col;
}

void RenderTextLogic::ET_setText(const char* str) {
    text = str;
    calcTextAABB();
    if(!text.empty()) {
        ETNode<ETRenderEvents>::connect(getEntityId());
    } else {
        ETNode<ETRenderEvents>::disconnect();
    }
}
