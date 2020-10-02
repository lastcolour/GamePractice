#include "Game/ViewScripts/LevelsViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIView.hpp"
#include "Game/ETLevelSelector.hpp"
#include "Core/ETLogger.hpp"

void LevelsViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelsViewScript>("LevelsViewScript")) {
    }
}

LevelsViewScript::LevelsViewScript() {
}

LevelsViewScript::~LevelsViewScript() {
}

void LevelsViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    } else if(event.type == UIEvent::EventType::OnLevelsStartLevel) {
        std::string levelName;
        ET_SendEventReturn(levelName, event.senderId, &ETLevelSelector::ET_getLevelName);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    }
}