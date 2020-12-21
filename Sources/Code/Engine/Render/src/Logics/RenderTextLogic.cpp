#include "Logics/RenderTextLogic.hpp"
#include "RenderFont.hpp"
#include "Entity/ETEntity.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderManager.hpp"
#include "Nodes/TextNode.hpp"

#include <algorithm>
#include <cassert>

RenderTextLogic::RenderTextLogic() :
    RenderNode(RenderNodeType::Text),
    color(255, 255, 255),
    fontHeight(24),
    isTextChanged(true),
    isColorChanged(true) {
}

RenderTextLogic::~RenderTextLogic() {
}

void RenderTextLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderTextLogic>("RenderText")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("color", &RenderTextLogic::color);
        classInfo->addField("fontSize", &RenderTextLogic::fontHeight);
        classInfo->addField("text", &RenderTextLogic::text);
    }
}

void RenderTextLogic::init() {
    RenderNode::init();
    ETNode<ETRenderTextLogic>::connect(getEntityId());
    ET_SendEventReturn(font, &ETRenderFontManager::ET_getDefaultFont);
    ET_setColor(color);
    ET_setText(text.c_str());
    ET_setFontHeight(fontHeight);
}

void RenderTextLogic::ET_setFontHeight(int newFontHeight) {
    fontHeight = newFontHeight;
    isTextChanged = true;
    markForSyncWithRender();
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    auto textSize = font->getTextSize(text);
    auto fontScale = fontHeight / static_cast<float>(font->getHeight());

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(textSize.x * tm.scale.x * fontScale, textSize.y * tm.scale.y * fontScale);
    aabb.setCenter(Vec2(tm.pt.x, tm.pt.y));

    return aabb;
}

AABB2Di RenderTextLogic::ET_getTextAABBi() const {
    auto box = ET_getTextAABB();

    AABB2Di resBox;
    resBox.bot = Vec2i(static_cast<int>(box.bot.x), static_cast<int>(box.bot.y));
    resBox.top = Vec2i(static_cast<int>(box.top.x), static_cast<int>(box.top.y));
    return resBox;
}

void RenderTextLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    isColorChanged = true;
    markForSyncWithRender();
}

void RenderTextLogic::ET_setText(const char* str) {
    text = str;
    isTextChanged = true;
    markForSyncWithRender();
}

void RenderTextLogic::onSyncWithRender() {
    auto textProxyNode = static_cast<TextNode*>(proxyNode);
    if(isTextChanged) {
        isTextChanged = false;
        textProxyNode->setText(text);
        textProxyNode->setFontHeight(fontHeight);
    }
    if(isColorChanged) {
        isColorChanged = false;
        textProxyNode->setColor0(color);
    }
}