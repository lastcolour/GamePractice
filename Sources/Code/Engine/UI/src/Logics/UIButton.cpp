#include "Logics/UIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "UI/ETUIAnimation.hpp"

void UIButton::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UIEventType>("UIEventType")) {
        enumInfo->addValues<UIEventType>({
            {"None", UIEventType::None},
            {"OnStartGame", UIEventType::OnStartGame},
            {"OnBackButton", UIEventType::OnBackButton},
        });
    }
    if(auto classInfo = ctx.classInfo<UIButton>("UIButton")) {
        classInfo->addField("event", &UIButton::eventType);
        classInfo->addField("label", &UIButton::labelId);
        classInfo->addField("box", &UIButton::renderId);
    }
}

UIButton::UIButton() :
    eventType(UIEventType::None),
    isHovered(false) {
}

UIButton::~UIButton() {
}

bool UIButton::init() {
    UIBoxVisual::init();
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIAnimationEvents>::connect(getEntityId());
    return true;
}

void UIButton::deinit() {
    UIBoxVisual::deinit();
    ETNode<ETUIInteractionBox>::disconnect();
}

void UIButton::ET_onPress() {
    if(eventType == UIEventType::None) {
        return;
    }
    if(!ET_IsExistNode<ETUIAnimation>(getEntityId())) {
        ET_SendEvent(&ETUIEventManager::ET_onEvent, eventType);
    } else {
        ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, getEntityId());
        ET_SendEvent(&ETUIAnimation::ET_start);
    }
}

bool UIButton::ET_isHovered() const {
    return isHovered;
}

void UIButton::ET_onHover(bool flag) {
    isHovered = flag;
}

AABB2Di UIButton::ET_getHitBox() const {
    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getBox);
    return box;
}

void UIButton::ET_onAnimationEnd() {
    ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, InvalidEntityId);
    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventType);
}