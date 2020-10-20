#include "Logics/UILabel.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderNode.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Core/ETLogger.hpp"
#include "UIUtils.hpp"

void UILabel::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILabel>("UILabel")) {
        classInfo->addField("style", &UILabel::style);
        classInfo->addField("text", &UILabel::text);
        classInfo->addField("render", &UILabel::labelRenderId);
    }
}

UILabel::UILabel() {
}

UILabel::~UILabel() {
}

bool UILabel::init() {
    UIElement::init();
    ETNode<ETUILabel>::connect(getEntityId());
    ETNode<ETUIViewPortEvents>::connect(getEntityId());
    if(ET_IsExistNode<ETRenderTextLogic>(labelRenderId)) {
        ET_setTextRender(labelRenderId);
    }
    return true;
}

void UILabel::deinit() {
    UIElement::deinit();
}

void UILabel::ET_setText(const char* newText) {
    text = newText;
    ET_SendEvent(labelRenderId, &ETRenderTextLogic::ET_setText, newText);
    updateHostLayout();
}

const char* UILabel::ET_getText() const {
    return text.c_str();
}

float UILabel::ET_getFontSize() const {
    return style.fontSize;
}

void UILabel::ET_setFontSize(float newFontSize) {
    style.fontSize = newFontSize;
    auto fontHeight = ET_getShared<UIConfig>()->getSizeOnGrind(style.fontSize);
    ET_SendEvent(labelRenderId, &ETRenderTextLogic::ET_setFontHeight, fontHeight);
    updateHostLayout();
}

AABB2Di UILabel::ET_getBox() const {
    AABB2Di textBox(Vec2i(0), Vec2i(0));
    ET_SendEventReturn(textBox, labelRenderId, &ETRenderTextLogic::ET_getTextAABBi);
    return textBox;
}

UIBoxMargin UILabel::ET_getMargin() const {
    return UI::CalculateMargin(getEntityId(), style.margin);
}

void UILabel::ET_setTextRender(EntityId newRenderId) {
    labelRenderId = newRenderId;
    if(!labelRenderId.isValid()) {
        return;
    }
    if(!ET_IsExistNode<ETRenderTextLogic>(labelRenderId)) {
        LogWarning("[UILabel::ET_setTextRender] Can't find text renderer in entity: '%s'",
            EntityUtils::GetEntityName(labelRenderId));
        return;
    }
    ET_setText(text.c_str());
    ET_setFontSize(style.fontSize);
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
}

void UILabel::onZIndexChanged(int newZIndex) {
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setDrawPriority, newZIndex);
}

void UILabel::ET_onViewPortChanged(const Vec2i& newSize) {
    ET_setFontSize(style.fontSize);
}

void UILabel::onHide(bool flag) {
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setVisibilityMultiplier, !flag);

}

void UILabel::onAlphaChanged(float newAlpha) {
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void UILabel::ET_onLoaded() {
    if(labelRenderId != getEntityId()) {
        return;
    }
    ET_setTextRender(labelRenderId);
}