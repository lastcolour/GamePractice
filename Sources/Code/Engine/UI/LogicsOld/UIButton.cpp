#include "UI/Logics/UIButton.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* BUTTON_PRESS_ANIMATION = "UI/Default/ButtonPressAnimation.json";

} // namespace

UIButton::UIButton() :
    isHovered(false) {
}

UIButton::~UIButton() {
}

void UIButton::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIButton>("UIButton")) {
        classInfo->addField("event", &UIButton::eventName);
    }
}

void UIButton::ET_onPress() {
    bool isSomeButtonPressed = false;
    ET_SendEventReturn(isSomeButtonPressed, &ETUIButtonEventManager::ET_isSomeButtonPressed);
    if(isSomeButtonPressed) {
        return;
    }

    ET_SendEvent(&ETUIButtonEventManager::ET_setButtonPressed, true);
    ET_SendEvent(animEntityId, &ETUIButtonPressAnimation::ET_startPressAnimation);
}

void UIButton::ET_onPressAnimationEnd() {
    ET_SendEvent(&ETUIButtonEventManager::ET_setButtonPressed, false);
    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventName.c_str());
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

void UIButton::deinit() {
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
