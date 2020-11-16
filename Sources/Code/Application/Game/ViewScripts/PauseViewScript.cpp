#include "Game/ViewScripts/PauseViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIView.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Audio/ETEqualizer.hpp"

void PauseViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<PauseViewScript>("PauseViewScript")) {
    }
}

PauseViewScript::PauseViewScript() {
}

PauseViewScript::~PauseViewScript() {
}

void PauseViewScript::ET_onViewOpened() {
    ET_SendEvent(getEntityId(), &ETEqualizer::ET_enable);
}

void PauseViewScript::ET_onViewClosed() {
    ET_SendEvent(getEntityId(), &ETEqualizer::ET_disable);
}

void PauseViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnPauseViewResume || event.type == UIEvent::EventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
    } else if(event.type == UIEvent::EventType::OnPauseViewRestart) {
        ET_SendEvent(&ETGameStateManager::ET_finishGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);

        std::string levelName;
        ET_SendEventReturn(levelName, &ETGameBoardSpawner::ET_getLevelName);
        ET_QueueEvent(&ETGameBoardSpawner::ET_setPendingLevel, levelName);

        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    } else if(event.type == UIEvent::EventType::OnPauseViewExit) {
        ET_SendEvent(&ETGameStateManager::ET_finishGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    }
}
