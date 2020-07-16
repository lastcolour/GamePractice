#include "Logics/UILabel.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIConfig.hpp"
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
    ETNode<ETRenderCameraEvents>::connect(getEntityId());
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
    updateLayout();
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
    updateLayout();
}

AABB2Di UILabel::ET_getBox() const {
    AABB2D box(Vec2(0.f), Vec2(0.f));
    ET_SendEventReturn(box, labelRenderId, &ETRenderTextLogic::ET_getTextAABB);

    AABB2Di textBox;
    textBox.bot = Vec2i(static_cast<int>(box.bot.x), static_cast<int>(box.bot.y));
    textBox.top = Vec2i(static_cast<int>(box.top.x + 0.5f), static_cast<int>(box.top.y + 0.5f));
    return textBox;
}

UIBoxMargin UILabel::ET_getMargin() const {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    return UI::CalculateMargin(style.margin, tm);
}

void UILabel::ET_setTextRender(EntityId newRenderId) {
    labelRenderId = newRenderId;
    if(!labelRenderId.isValid()) {
        return;
    }
    if(!ET_IsExistNode<ETRenderTextLogic>(labelRenderId)) {
        LogWarning("[UILabel::ET_setTextRender] Can't find text renderer in entity: %d", labelRenderId.getRawId());
        return;
    }
    ET_setText(text.c_str());
    ET_setFontSize(style.fontSize);
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setDrawPriority, ET_getZIndex());
}

void UILabel::onZIndexChanged(int newZIndex) {
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setDrawPriority, newZIndex);
}

void UILabel::ET_onRenderPortResized() {
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
    ET_SendEvent(labelRenderId, &ETRenderNode::ET_setAlpha, newAlpha);
}

void UILabel::ET_onAllLogicsCreated() {
    if(labelRenderId != getEntityId()) {
        return;
    }
    ET_setTextRender(labelRenderId);
}