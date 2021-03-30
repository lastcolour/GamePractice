#include "Logics/RenderTextLogic.hpp"
#include "RenderFont.hpp"
#include "Nodes/TextNode.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

RenderTextLogic::RenderTextLogic() :
    RenderNode(RenderNodeType::Text),
    color(255, 255, 255),
    fontHeight(24.f) {
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

void RenderTextLogic::onInit() {
    auto textProxyNode = static_cast<TextNode*>(proxyNode);
    textProxyNode->setFontHeight(fontHeight);
    textProxyNode->setColor0(color);
    textProxyNode->setText(text);

    ET_SendEventReturn(font, &ETRenderFontManager::ET_getDefaultFont);

    ETNode<ETRenderTextLogic>::connect(getEntityId());
}

void RenderTextLogic::ET_setFontHeight(float newFontHeight) {
    assert(newFontHeight >= 0.f && "Invalid font height");

    fontHeight = newFontHeight;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newFontHeight](){
        auto textProxyNode = static_cast<TextNode*>(node);
        textProxyNode->setFontHeight(newFontHeight);
    });
}

AABB2D RenderTextLogic::ET_getTextAABB() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    auto textSize = font->getTextSize(text);
    auto fontScale = fontHeight / static_cast<float>(font->getHeight());

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(textSize.x * tm.scale.x * fontScale, textSize.y * tm.scale.y * fontScale);
    aabb.setCenter(tm.pt.x, tm.pt.y);

    return aabb;
}

void RenderTextLogic::ET_setColor(const ColorB& newColor) {
    color = newColor;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newColor](){
        auto textProxyNode = static_cast<TextNode*>(node);
        textProxyNode->setColor0(newColor);
    });
}

void RenderTextLogic::ET_setText(const char* str) {
    text = str;
    std::string copyText = text;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, s(std::move(copyText))](){
        auto textProxyNode = static_cast<TextNode*>(node);
        textProxyNode->setText(std::move(s));
    });
}