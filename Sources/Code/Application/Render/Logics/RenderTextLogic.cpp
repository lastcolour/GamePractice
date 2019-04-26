#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderFont.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"

#include <algorithm>

namespace {
    const char* TEXT_RENDER_MATERIAL = "text_solid_color";
} // namespace

RenderTextLogic::RenderTextLogic() {
}

RenderTextLogic::~RenderTextLogic() {
}

bool RenderTextLogic::serialize(const JSONNode& node) {
    return true;
}

void RenderTextLogic::createVAO() {
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 6, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), static_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool RenderTextLogic::init() {
    ET_SendEventReturn(font, &ETRender::ET_createDefaultFont);
    if(!font) {
        return false;
    }
    ET_SendEventReturn(mat, &ETRender::ET_createMaterial, TEXT_RENDER_MATERIAL);
    if(!mat) {
        return false;
    }
    createVAO();
    ETNode<ETRenderTextLogic>::connect(getEntityId());
    return true;
}

void RenderTextLogic::ET_onRender(const RenderContext& renderCtx) {
    Mat4 mvp = getModelMat();
    mvp = renderCtx.proj2dMat * mvp;

    mat->bind();
    mat->setTexture2D("tex", font->getTexId());
    mat->setUniformMat4("MVP", mvp);
    mat->setUniform4f("color", ColorB(0, 0, 255));

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
    const auto scale = Vec2(tm.scale.x, tm.scale.y);
    Vec2 pt = Vec2(aabb.bot.x, aabb.top.y);

    glBindVertexArray(vaoId);
    for(auto ch : text) {
        if(auto glyph = font->getGlyph(ch)) {
            Vec2 glyphPt(0);
            glyphPt.x = pt.x + glyph->bearing.x * scale.x;
            glyphPt.y = pt.y - glyph->bearing.y * scale.y;
            const float w = glyph->size.x * scale.x;
            const float h = glyph->size.y * scale.y;

            const auto& txBot = glyph->texCoords.bot;
            const auto& txTop = glyph->texCoords.top;

            Vec4 vertex[6];
            // First tri
            vertex[0] = { glyphPt.x    , glyphPt.y + h, txBot.x, txTop.y };
            vertex[1] = { glyphPt.x + w, glyphPt.y    , txTop.x, txBot.y };
            vertex[2] = { glyphPt.x    , glyphPt.y    , txBot.x, txBot.y };
            // Second tri
            vertex[3] = { glyphPt.x    , glyphPt.y + h, txBot.x, txTop.y };
            vertex[4] = { glyphPt.x + w, glyphPt.y + h, txTop.x, txTop.y };
            vertex[5] = { glyphPt.x + w, glyphPt.y    , txTop.x, txBot.y };

            pt.x += glyph->advance.x * scale.x;
            pt.y += glyph->advance.y * scale.y;

            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), &vertex);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glBindVertexArray(0);

    mat->unbind();
}

void RenderTextLogic::calcTextAABB() {
    Vec2i botPt(0);
    for(size_t i = 0, sz = text.size(); i < sz; ++i) {
        if(auto glyph = font->getGlyph(text[i])) {
            botPt.x += std::max(glyph->advance.x, glyph->size.x);
            botPt.y = glyph->advance.y;
        }
    }
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(static_cast<float>(botPt.x),
        static_cast<float>(botPt.y));

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    return aabb;
}

Mat4 RenderTextLogic::getModelMat() const {
    Transform tm;
    ET_SendEventReturn(tm, &ETGameObject::ET_getTransform);
    Mat4 model(1.f);
    const Vec3 center = Vec3(aabb.getCenter(), 1.f);
    Math::Translate(model, center);
    Math::Rotate(model, tm.quat);
    Math::Translate(model, center);
    Math::Scale(model, tm.scale);
    return model;
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
