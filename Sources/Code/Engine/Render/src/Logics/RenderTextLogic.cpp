#include "Logics/RenderTextLogic.hpp"
#include "RenderFont.hpp"
#include "Entity/ETEntity.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderManager.hpp"
#include "Nodes/ETRenderProxyNode.hpp"

#include <algorithm>
#include <cassert>

RenderTextLogic::RenderTextLogic() :
    RenderNode(RenderNodeType::Text),
    color(255, 255, 255),
    fontHeight(24) {
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

bool RenderTextLogic::init() {
    RenderNode::init();
    ETNode<ETRenderTextLogic>::connect(getEntityId());
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createDefaultFont);
    return true;
}

void RenderTextLogic::ET_setFontHeight(int newFontHeight) {
    fontHeight = newFontHeight;
    ET_SendEvent(renderNodeId, &ETRenderProxyNode::ET_setFontHeight, newFontHeight);
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

void RenderTextLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    ET_SendEvent(renderNodeId, &ETRenderProxyNode::ET_setColor0, newColor);
}

void RenderTextLogic::ET_setText(const char* str) {
    text = str;
    ET_SendEvent(renderNodeId, &ETRenderProxyNode::ET_setText, text);
}