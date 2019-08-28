#include "UI/Logics/UIButton.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

UIButton::UIButton() :
    isHovered(false) {
}

UIButton::~UIButton() {
}

void UIButton::ET_onPress() {
    ColorB col;
    col.r = rand() % 256;
    col.g = rand() % 256;
    col.b = rand() % 256;
    UIStyle newStyle = ET_getStyle();
    newStyle.color = col;
    ET_setStyle(newStyle);

    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventName.c_str());
}

bool UIButton::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UIButton::serialize] Can't serialize UIBox");
        return false;
    }
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
    UIBox::init();
    ETNode<ETUIButton>::connect(getEntityId());
    ETNode<ETUIInteractionBox>::connect(getEntityId());
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