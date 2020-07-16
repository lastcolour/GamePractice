#include "Logics/UIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Render/ETRenderNode.hpp"

#include <cassert>

void UIButton::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UIEventType>("UIEventType")) {
        enumInfo->addValues<UIEventType>({
            {"None", UIEventType::None},
            {"OnStartGame", UIEventType::OnStartGame},
            {"OnBackButton", UIEventType::OnBackButton},
        });
    }
    if(auto classInfo = ctx.classInfo<UIButton>("UIButton")) {
        classInfo->addBaseClass<UIBox>();
        classInfo->addField("label", &UIButton::labelId);
        classInfo->addField("event", &UIButton::eventType);
    }
}

UIButton::UIButton() :
    eventType(UIEventType::None),
    isHovered(false) {
}

UIButton::~UIButton() {
}

bool UIButton::init() {
    UIBox::init();
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIAnimationEvents>::connect(getEntityId());
    return true;
}

void UIButton::deinit() {
    UIBox::deinit();
    ETNode<ETUIInteractionBox>::disconnect();
    ETNode<ETUIAnimationEvents>::disconnect();
}

void UIButton::ET_onPress() {
    EntityId activeBtId;
    ET_SendEventReturn(activeBtId, &ETUIButtonEventManager::ET_getActiveButton);
    if(activeBtId.isValid()) {
        return;
    }
    if(!ET_IsExistNode<ETUIAnimation>(getEntityId())) {
        ET_SendEvent(&ETUIEventManager::ET_onEvent, eventType);
    } else {
        ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, getEntityId());
        ET_SendEvent(getEntityId(), &ETUIAnimation::ET_start);
    }
}

bool UIButton::ET_isHovered() const {
    return isHovered;
}

void UIButton::ET_onHover(bool flag) {
    isHovered = flag;
}

AABB2Di UIButton::ET_getHitBox() const {
    return ET_getBox();
}

void UIButton::ET_onAnimationEnd() {
    {
        EntityId activeBtId;
        ET_SendEventReturn(activeBtId, &ETUIButtonEventManager::ET_getActiveButton);
        assert(activeBtId == getEntityId());
    }
    ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, InvalidEntityId);
    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventType);
}

void UIButton::onZIndexChanged(int newZIndex) {
    UIBox::onZIndexChanged(newZIndex);
    auto labelZIndex = newZIndex + 1;
    ET_SendEvent(labelId, &ETUIElement::ET_setZIndex, labelZIndex);
    ET_SendEvent(labelId, &ETRenderNode::ET_setDrawPriority, labelZIndex);
}

int UIButton::ET_getZIndexDepth() const {
    return 2;
}

void UIButton::onHide(bool flag) {
    UIBox::onHide(flag);
    if(flag) {
        ET_SendEvent(labelId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(labelId, &ETRenderNode::ET_show);
    }
}

void UIButton::onAlphaChanged(float newAlpha) {
    UIBox::onAlphaChanged(newAlpha);
    ET_SendEvent(labelId, &ETRenderNode::ET_setAlpha, newAlpha);
}

void UIButton::onDisabled(bool flag) {
    if(flag) {
        ETNode<ETUIInteractionBox>::disconnect();
    } else {
        ETNode<ETUIInteractionBox>::connect(getEntityId());
    }
}

void UIButton::ET_onAllLogicsCreated() {
    UIBox::ET_onAllLogicsCreated();
    if(labelId == getEntityId()) {
        auto labelZIndex = ET_getZIndex() + 1;
        ET_SendEvent(labelId, &ETUIElement::ET_setZIndex, labelZIndex);
        ET_SendEvent(labelId, &ETRenderNode::ET_setDrawPriority, labelZIndex);
    }
}