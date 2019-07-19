#include "UI/Logics/UIButton.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "Game/ETGameInterfaces.hpp"

namespace {

EntityId createUIObject(const JSONNode& node) {
    std::string objName;
    node.value("object", objName);
    EntityId entId;
    ET_SendEventReturn(entId, &ETGameObjectManager::ET_createGameObject, objName);
    if(!entId.isValid()) {
        return entId;
    }
    JSONNode styleNode = node.object("style");
    if(styleNode) {
        UIStyle style;
        ET_SendEventReturn(style, entId, &ETUIBox::ET_getStyle);
        style.serialize(styleNode);
        ET_SendEvent(entId, &ETUIBox::ET_setStyle, style);
    }
    return entId;
}

} // namespace

UIButton::UIButton() {
}

UIButton::~UIButton() {
}

void UIButton::ET_onPress() {
    ColorB col;
    col.r = rand() % 256;
    col.g = rand() % 256;
    col.b = rand() % 256;
    UIStyle style = ET_getStyle();
    style.color = col;
    ET_setStyle(style);

    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventName);
}

bool UIButton::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UIButton::serialize] Can't serialize UIBox");
        return false;
    }
    node.value("event", eventName);
    auto labelNode = node.object("label");
    if(!labelNode) {
        LogWarning("[UIButton::serialize] Can't find label node");
        return false;
    }
    labelEntId = createUIObject(labelNode);
    if(!labelEntId.isValid()) {
        LogWarning("[UIButton::serialize] Can't create label object");
        return false;
    }
    std::string labelText;
    labelNode.value("text", labelText);
    ET_SendEvent(labelEntId, &ETUILabel::ET_setText, labelText);
    ET_SendEvent(labelEntId, &ETGameObject::ET_setParent, getEntityId());
    return true;
}

bool UIButton::init() {
    if(!UIBox::init()) {
        LogError("[UIButton::init] Can't init box");
        return false;
    }
    ETNode<ETUIButton>::connect(getEntityId());
    return true;
}