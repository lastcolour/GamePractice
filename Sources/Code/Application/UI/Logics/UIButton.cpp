#include "UI/Logics/UIButton.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

namespace {

const char* BUTTON_PRESS_ANIMATION = "UI/Default/ButtonPressAnimation.json";

} // namespace

UIButton::UIButton() :
    isHovered(false) {
}

UIButton::~UIButton() {
}

void UIButton::ET_onPress() {
    ET_SendEvent(animEntityId, &ETUIButtonPressAnimation::ET_startPressAnimation);
}

void UIButton::ET_onPressAnimationEnd() {
    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventName.c_str());
}

bool UIButton::serialize(const JSONNode& node) {
    UIBox::serialize(node);
    node.read("event", eventName);
    return true;
}

void UIButton::ET_setEventName(const char* newEventName) {
    if(!newEventName || !newEventName[0]) {
        LogWarning("[UIButton::ET_setEventName] Set empty event name for button: %s", getEntityName());
    }
    eventName = newEventName;
}

bool UIButton::init() {
    ET_SendEventReturn(animEntityId, &ETEntityManager::ET_createEntity, BUTTON_PRESS_ANIMATION);
    if(animEntityId.isValid()) {
        ET_SendEvent(getEntityId(), &ETEntity::ET_addChild, animEntityId);
    } else {
        LogError("[UIButton::init] Can't create press animation logic: %s", BUTTON_PRESS_ANIMATION);
        return false;
    }
    UIBox::init();
    ETNode<ETUIButton>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIButtonPressAnimationEvents>::connect(getEntityId());
    return true;
}

void UIButton::ET_onHover(bool flag) {
    isHovered = flag;
}

bool UIButton::ET_isHovered() const {
    return isHovered;
}

AABB2Di UIButton::ET_getHitBox() const {
    return ET_getAabb2di();
}

float UIButton::ET_getPressDuration() const {
    float pressDuration = 0.f;
    ET_SendEventReturn(pressDuration, animEntityId, &ETUIButtonPressAnimation::ET_getAnimationDuration);
    return pressDuration;
}
