#include "Logics/RenderTextLogic.hpp"
#include "RenderFont.hpp"
#include "Render/ETRenderManager.hpp"

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
    textCmd->fontHeight = fontHeight;
    textCmd->color = color;
    textCmd->text = text;
    textCmd->font = font;

    ETNode<ETRenderTextLogic>::connect(getEntityId());
}

void RenderTextLogic::ET_setFontHeight(float newFontHeight) {
    assert(newFontHeight >= 0.f && "Invalid font height");
    fontHeight = newFontHeight;
    DrawTextCmd::QueueFontHeightUpdate(*cmd, fontHeight);
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    AABB2D aabb(0.f);
    if(!font) {
        return aabb;
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

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