#include "UI/Logics/UIButton.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "Game/GameETInterfaces.hpp"

UIButton::UIButton() {
}

UIButton::~UIButton() {
}

void UIButton::ET_onPress() {
}

bool UIButton::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UIButton::serialize] UIBox searilization failed");
        return false;
    }
    node.value("text", text);
    return true;
}

bool UIButton::init() {
    if(!UIBox::init()) {
        LogWarning("[UIButton::init] UIBox init failed");
        return false;
    }
    if(!text.empty()) {
    }
    return true;
}