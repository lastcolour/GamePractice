#include "Game/ViewScripts/MainViewScript.hpp"
#include "Game/ETGameMusic.hpp"
#include "UI/ETUIView.hpp"

void MainViewScript::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<MainViewScript>("MainViewScript")) {
        (void)classInfo;
    }
}

MainViewScript::MainViewScript() {
}

MainViewScript::~MainViewScript() {
}

void MainViewScript::ET_onViewGetFocus() {
    BaseViewScript::ET_onViewGetFocus();
    ET_SendEvent(&ETGameMusic::ET_setTheme, EMusicTheme::Menu);
}

void MainViewScript::onEvent(const UIEvent& event) {
    if(event.type == UIEvent::EventType::OnMainViewStartGame) {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Main);
        ET_SendEvent(&ETUIViewManager::ET_openView, UIViewType::Levels);
    }
}
