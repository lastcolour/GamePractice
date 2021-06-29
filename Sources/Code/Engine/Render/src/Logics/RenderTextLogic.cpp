#include "Logics/RenderTextLogic.hpp"
#include "RenderFont.hpp"
#include "Nodes/TextNode.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

RenderTextLogic::RenderTextLogic() :
    RenderNode(RenderNodeType::Text),
    color(255, 255, 255),
    fontHeight(24.f),
    fontType(EFontType::Game) {
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
    textProxyNode->setFontType(fontType);
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createFont, EFontType::Game);

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

void RenderTextLogic::ET_setFontType(EFontType newFontType) {
    fontType = newFontType;
    ET_SendEventReturn(font, &ETRenderFontManager::ET_createFont, fontType);
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, fontT=newFontType](){
        auto textProxyNode = static_cast<TextNode*>(node);
        textProxyNode->setFontType(fontT);
    });
}

EFontType RenderTextLogic::ET_getFontType() const {
    return fontType;
}