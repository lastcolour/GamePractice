#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/RenderMaterial.hpp"
#include "Render/RenderFont.hpp"
#include "Math/MatrixTransform.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"

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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Vec2 RenderTextLogic::initVertexForGlyph(const Vec2& pos, const RenderGlyph& glyph) {
    const Vec2 scale(1.f);
    Vec2 pt(0);
    pt.x = pos.x + glyph.bearing.x * scale.x;
    pt.y = pos.y - glyph.bearing.y * scale.y;
    const float w = glyph.size.x * scale.x;
    const float h = glyph.size.y * scale.y;

    const Vec2i textSize = font->getTexSize();
    Vec2 txBot(0.f);
    txBot.x = glyph.offset;
    txBot.y = 0.f;

    Vec2 txTop(1.f, 1.f);
    txTop.x = glyph.offset + glyph.bearing.x / static_cast<float>(textSize.x);
    txTop.y = glyph.bearing.y / static_cast<float>(textSize.y);

    Vec4 vertex[6];
    // First tri
    vertex[0] = { pt.x    , pt.y + h, txBot.x, txTop.y };
    vertex[1] = { pt.x + w, pt.y    , txTop.x, txBot.y };
    vertex[2] = { pt.x    , pt.y    , txBot.x, txBot.y };
    // Second tri
    vertex[3] = { pt.x    , pt.y + h, txBot.x, txTop.y };
    vertex[4] = { pt.x + w, pt.y + h, txTop.x, txTop.y };
    vertex[5] = { pt.x + w, pt.y    , txTop.x, txBot.y };

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), &vertex); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Vec2 nextPos = pos;
    nextPos.x += glyph.advance.x * scale.x;
    nextPos.y += glyph.advance.y * scale.y;
    return nextPos;
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

    glBindVertexArray(vaoId);

    Vec2 pt = Vec2(50.f, 240.f);
    for(auto ch : text) {
        if(auto glyph = font->getGlyph('A')) {
            pt = initVertexForGlyph(pt, *glyph);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glBindVertexArray(0);

    mat->unbind();
}

void RenderTextLogic::calcTextAABB() {
    Vec2i botPt(0);
    for(auto ch : text) {
        if(auto glyph = font->getGlyph(ch)) {
            botPt.x += glyph->advance.x;
            botPt.y += glyph->advance.y;
        }
    }
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(static_cast<float>(botPt.x), 
        static_cast<float>(botPt.y));
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
    if(!text.empty()) {
        calcTextAABB();
        ETNode<ETRenderEvents>::connect(getEntityId());
    } else {
        ETNode<ETRenderEvents>::disconnect();
    }
}
