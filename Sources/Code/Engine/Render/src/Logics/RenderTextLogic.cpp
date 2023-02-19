#include "Logics/RenderTextLogic.hpp"
#include "RenderFont.hpp"
#include "Render/ETRenderManager.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

RenderTextLogic::RenderTextLogic() :
    DrawCommandProxy(EDrawCmdType::Text),
    color(255, 255, 255),
    fontHeight(24.f),
    fontType(EFontType::Game) {
}

RenderTextLogic::~RenderTextLogic() {
}

void RenderTextLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderTextLogic>("RenderText")) {
        classInfo->addBaseClass<DrawCommandProxy>();
        classInfo->addField("color", &RenderTextLogic::color);
        classInfo->addField("fontSize", &RenderTextLogic::fontHeight);
        classInfo->addField("text", &RenderTextLogic::text);
    }
}

void RenderTextLogic::onInit() {
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createFont, EFontType::Game);

    auto textCmd = static_cast<DrawTextCmd*>(cmd);
    textCmd->color = color;
    textCmd->text = text;
    textCmd->font = font;
    textCmd->alignAtCenter = true;
    textCmd->updateTextMetric();

    ETNode<ETRenderTextLogic>::connect(getEntityId());
}

Mat4 RenderTextLogic::calcModelMat() const {
    float fontScale = 1.f;
    if(font) {
        fontScale = fontHeight / static_cast<float>(font->getHeight());
    }
    Mat4 mat = getTransform().toMat4();
    Vec3 scale(fontScale * normScale, fontScale * normScale, 1.f);
    Math::AddScale3D(mat, scale);
    return mat;
}

void RenderTextLogic::ET_setFontHeight(float newFontHeight) {
    assert(newFontHeight > 0.f && "Invalid font height");
    DrawCmd::QueueSizeUpdate(*cmd, Vec2(fontHeight),  Vec2(newFontHeight), cmdType);
    fontHeight = newFontHeight;
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    AABB2D aabb(0.f);
    if(!font) {
        return aabb;
    }

    const Transform& tm = getTransform();

    auto textSize = font->getTextSize(text);
    auto scale = fontHeight / static_cast<float>(font->getHeight());
    scale *= normScale;

    aabb.top = Vec2(textSize.x * tm.scale.x * scale, textSize.y * tm.scale.y * scale);
    aabb.setCenter(tm.pt.x, tm.pt.y);

    return aabb;
}

void RenderTextLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    DrawTextCmd::QueueColorUpdate(*cmd, color);
}

void RenderTextLogic::ET_setText(const char* str) {
    text = str;
    DrawTextCmd::QueueTextUpdate(*cmd, text);
}

void RenderTextLogic::ET_setFontType(EFontType newFontType) {
    fontType = newFontType;
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createFont, fontType);

    DrawTextCmd::QueueFontTypeUpdate(*cmd, fontType);
}

EFontType RenderTextLogic::ET_getFontType() const {
    return fontType;
}