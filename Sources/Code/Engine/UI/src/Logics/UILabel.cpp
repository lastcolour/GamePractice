#include "Logics/UILabel.hpp"
#include "Render/ETRenderNode.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Core/GlobalData.hpp"
#include "UIUtils.hpp"

void UILabel::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UILabel>("UILabel")) {
        classInfo->addBaseClass<UIElement>();
        classInfo->addField("style", &UILabel::style);
        classInfo->addField("text", &UILabel::text);
    }
}

UILabel::UILabel() {
}

UILabel::~UILabel() {
}

void UILabel::init() {
    UIElement::init();
    ETNode<ETUIElementBox>::connect(getEntityId());
    ETNode<ETUILabel>::connect(getEntityId());
    ETNode<ETUIViewPortEvents>::connect(getEntityId());

    labelRenderId = getEntityId();
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
    auto fontHeight = GetGlobal<UIConfig>()->getSizeOnGrid(style.fontSize);
    ET_SendEvent(labelRenderId, &ETRenderTextLogic::ET_setFontHeight, fontHeight);
    updateHostLayout();
}

AABB2D UILabel::ET_getBox() const {
    AABB2D textBox(0.f);
    ET_SendEventReturn(textBox, labelRenderId, &ETRenderTextLogic::ET_getTextAABB);
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
    if(flag) {
        ET_SendEvent(labelRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(labelRenderId, &ETRenderNode::ET_show);
    }
}

void UILabel::onAlphaChanged(float newAlpha) {
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void UILabel::ET_onLoaded() {
    UIElement::ET_onLoaded();
    ET_setTextRender(labelRenderId);
}