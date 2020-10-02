#include "Game/ViewScripts/BaseViewScript.hpp"

BaseViewScript::BaseViewScript() :
    hasFocus(false) {
}

BaseViewScript::~BaseViewScript() {
}

bool BaseViewScript::init() {
    ETNode<ETUIViewScript>::connect(getEntityId());
    return true;
}

void BaseViewScript::deinit() {
}

void BaseViewScript::ET_onGetFocus() {
    hasFocus = true;
}

void BaseViewScript::ET_onLostFocus() {
    hasFocus = false;
}

void BaseViewScript::ET_onEvent(const UIEvent& event) {
    if(!hasFocus) {
        return;
    }
    onEvent(event);
}