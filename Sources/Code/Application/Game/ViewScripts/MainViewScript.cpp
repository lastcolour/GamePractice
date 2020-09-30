#include "Game/ViewScripts/MainViewScript.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UI/ETUIView.hpp"

void MainViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<MainViewScript>("MainViewScript")) {
    }
}

MainViewScript::MainViewScript() {
}

MainViewScript::~MainViewScript() {
}

void MainViewScript::onEvent(UIEventType eventType) {
    if(eventType == UIEventType::OnMainViewStartGame) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Main);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Game);
    }
}
