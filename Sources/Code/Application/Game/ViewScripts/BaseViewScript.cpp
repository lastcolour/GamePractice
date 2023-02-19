#include "Game/ViewScripts/BaseViewScript.hpp"

BaseViewScript::BaseViewScript() :
    hasFocusFlag(false) {
}

BaseViewScript::~BaseViewScript() {
}

void BaseViewScript::init() {
    ETNode<ETUIViewScript>::connect(getEntityId());
}

void BaseViewScript::deinit() {
}

void BaseViewScript::ET_onViewOpened() {
    LogDebug("[BaseViewScript::ET_onViewOpened] View: '%s'", getEntityName());
}

void BaseViewScript::ET_onViewClosed() {
}

void BaseViewScript::ET_onViewGetFocus() {
    hasFocusFlag = true;
}

void BaseViewScript::ET_onViewLostFocus() {
    hasFocusFlag = false;
}

void BaseViewScript::ET_onEvent(const UIEvent& event) {
    if(!hasFocusFlag) {
        return;
    }
    onEvent(event);
}

bool BaseViewScript::hasFocus() const {
    return hasFocusFlag;
}