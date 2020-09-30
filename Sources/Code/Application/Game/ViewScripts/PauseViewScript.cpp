#include "Game/ViewScripts/PauseViewScript.hpp"
#include "Game/ETGame.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIView.hpp"

void PauseViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<PauseViewScript>("PauseViewScript")) {
    }
}

PauseViewScript::PauseViewScript() {
}

PauseViewScript::~PauseViewScript() {
}

void PauseViewScript::onEvent(UIEventType eventType) {
    if(eventType == UIEventType::OnPauseViewResume || eventType == UIEventType::OnBackButton) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
    } else if(eventType == UIEventType::OnPauseViewRestart) {
        ET_SendEvent(&ETGameStateManager::ET_finishGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    } else if(eventType == UIEventType::OnPauseViewExit) {
        ET_SendEvent(&ETGameStateManager::ET_finishGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::PauseGame);
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Game);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Main);
    }
}
