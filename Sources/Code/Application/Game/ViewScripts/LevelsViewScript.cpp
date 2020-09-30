#include "Game/ViewScripts/LevelsViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIView.hpp"

void LevelsViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelsViewScript>("LevelsViewScript")) {
    }
}

LevelsViewScript::LevelsViewScript() {
}

LevelsViewScript::~LevelsViewScript() {
}

void LevelsViewScript::onEvent(UIEventType eventType) {
    if(eventType == UIEventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    }
}