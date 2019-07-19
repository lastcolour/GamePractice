#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderFont.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>

RenderTextLogic::RenderTextLogic() :
    vaoId(0),
    vboId(0),
    color(255, 255, 255) {
}

RenderTextLogic::~RenderTextLogic() {
    glDeleteBuffers(1, &vboId);
    glDeleteVertexArrays(1, &vaoId);
}

bool RenderTextLogic::serialize(const JSONNode& node) {
    std::string matName;
    node.value("mat", matName);
    ET_setMaterial(matName);
    return true;
}

void RenderTextLogic::createVAO() {
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 6, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool RenderTextLogic::init() {
    ET_SendEventReturn(font, &ETRender::ET_createDefaultFont);
    if(!mat) {
        return false;
    }
    if(!font) {
        return false;
    }
    createVAO();
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
    Vec2 pt = Vec2(aabb.bot.x, aabb.top.y);

    for(auto ch : text) {
        if(auto glyph = font->getGlyph(ch)) {
            Vec2 glyphPt(0);
            glyphPt.x = pt.x + glyph->bearing.x * scale.x;
            glyphPt.y = pt.y - glyph->bearing.y * scale.y;
            const float w = glyph->size.x * scale.x;
            const float h = glyph->size.y * scale.y;

            const auto& txBot = glyph->texCoords.bot;
            const auto& txTop = glyph->texCoords.top;

            Vec4 vertecies[6];
            // First tri
            vertecies[0] = { glyphPt.x    , glyphPt.y + h, txBot.x, txTop.y };
            vertecies[1] = { glyphPt.x + w, glyphPt.y    , txTop.x, txBot.y };
            vertecies[2] = { glyphPt.x    , glyphPt.y    , txBot.x, txBot.y };
            // Second tri
            vertecies[3] = { glyphPt.x    , glyphPt.y + h, txBot.x, txTop.y };
            vertecies[4] = { glyphPt.x + w, glyphPt.y + h, txTop.x, txTop.y };
            vertecies[5] = { glyphPt.x + w, glyphPt.y    , txTop.x, txBot.y };

            pt.x += glyph->advance.x * scale.x;
            pt.y += glyph->advance.y * scale.y;

            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertecies), &vertecies);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(vaoId);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
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

void RenderTextLogic::ET_setFontSize(size_t newFontSize) {
    fontSize = newFontSize;
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    return aabb;
}

void RenderTextLogic::ET_setMaterial(const std::string& matName) {
    ET_SendEventReturn(mat, &ETRender::ET_createMaterial, matName);
}

void RenderTextLogic::ET_setColor(const ColorB& col) {
    color = col;
}

void RenderTextLogic::ET_setText(const std::string& str) {
    text = str;
    calcTextAABB();
    if(!text.empty()) {
        ETNode<ETRenderEvents>::connect(getEntityId());
    } else {
        ETNode<ETRenderEvents>::disconnect();
    }
}
