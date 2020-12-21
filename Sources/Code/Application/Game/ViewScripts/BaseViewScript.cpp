#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"

BaseViewScript::BaseViewScript() :
    hasFocus(false) {
}

BaseViewScript::~BaseViewScript() {
}

void BaseViewScript::init() {
    ETNode<ETUIViewScript>::connect(getEntityId());
}

void BaseViewScript::deinit() {
}

void BaseViewScript::ET_onViewOpened() {
    LogDebug("[BaseViewScript::ET_onViewOpened] View: '%s'", EntityUtils::GetEntityName(getEntityId()));
}

void BaseViewScript::ET_onViewClosed() {
}

void BaseViewScript::ET_onViewGetFocus() {
    hasFocus = true;
}

void BaseViewScript::ET_onViewLostFocus() {
    hasFocus = false;
}

void BaseViewScript::ET_onEvent(const UIEvent& event) {
    if(!hasFocus) {
        return;
    }
    onEvent(event);
}