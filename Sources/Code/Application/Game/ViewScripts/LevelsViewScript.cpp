#include "Game/ViewScripts/LevelsViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIView.hpp"
#include "Core/ETLogger.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/ETLevelProgress.hpp"

void LevelsViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelsViewScript>("LevelsViewScript")) {
    }
}

LevelsViewScript::LevelsViewScript() {
}

LevelsViewScript::~LevelsViewScript() {
}

void LevelsViewScript::ET_onViewOpened() {
    BaseViewScript::ET_onViewOpened();
}

void LevelsViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    } else if(event.type == UIEvent::EventType::OnLevelsStartLevel) {
        std::string levelName;
        ET_SendEventReturn(levelName, &ETLevelButtonList::ET_getLevelNameForSender, event.senderId);
        if(levelName.empty()) {
            LogWarning("[LevelsViewScript::onEvent] Can't select level with empty name");
            return;
        }
        ET_QueueEvent(&ETGameBoardSpawner::ET_setPendingLevel, levelName);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Levels);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    }
}