#include "Logics/UILabel.hpp"
#include "Render/ETRenderNode.hpp"
#include "Config/UIConfig.hpp"
#include "Core/ETApplication.hpp"
#include "UI/UIBoxStyle.hpp"
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
    ETNode<ETUIElementGeom>::connect(getEntityId());
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

const UILabelStyle& UILabel::ET_getStyle() const {
    return style;
}

void UILabel::ET_setStyle(const UILabelStyle& newStyle) {
    style = newStyle;
    auto fontHeight = Core::GetGlobal<UIConfig>()->getSizeOnGrid(style.fontSize);
    ET_SendEvent(labelRenderId, &ETRenderTextLogic::ET_setFontHeight, fontHeight);
    updateHostLayout();
}

AABB2D UILabel::ET_getBox() const {
    AABB2D textBox(0.f);
    ET_SendEventReturn(textBox, labelRenderId, &ETRenderTextLogic::ET_getTextAABB);
    return textBox;
}

UIBoxMargin UILabel::ET_getMargin() const {
    return UI::CalculateMargin(getTransform(), style.margin);
}

void UILabel::ET_setRenderId(EntityId newRenderId) {
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
    ET_setStyle(style);
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setZIndex, ET_getZIndex());
    if(ET_isHidden()) {
        ET_SendEvent(labelRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(labelRenderId, &ETRenderNode::ET_show);
    }
}

EntityId UILabel::ET_getRenderId(EntityId newRenderId) const {
    return labelRenderId;
}

void UILabel::onZIndexChanged(int newZIndex) {
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setZIndex, newZIndex);
}

void UILabel::ET_onViewPortChanged(const Vec2i& newSize) {
    ET_setStyle(style);
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

void UILabel::onLoaded() {
    UIElement::onLoaded();
    ET_setRenderId(labelRenderId);
}