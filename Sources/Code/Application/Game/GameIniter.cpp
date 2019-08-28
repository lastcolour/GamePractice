#include "Game/GameIniter.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

GameIniter::GameIniter() {
}

GameIniter::~GameIniter() {
}

bool GameIniter::init() {
    bool isSurfaceValid = false;
    ET_SendEventReturn(isSurfaceValid, &ETSurface::ET_isValid);
    if(isSurfaceValid) {
        openMainView();
    }
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    return true;
}

void GameIniter::deinit() {
    ETNode<ETSurfaceEvents>::disconnect();
}

void GameIniter::ET_onSurfaceCreated() {
    if(!mainViewId.isValid()) {
        openMainView();
    }
}

void GameIniter::ET_onSurfaceDestroyed() {
    mainViewId = InvalidEntityId;
}

void GameIniter::openMainView() {
    const char* mainView = nullptr;
    ET_SendEventReturn(mainView, &ETGameConfig::ET_getMainViewName);
    if(!mainView || !mainView[0]) {
        LogError("[GameIniter::openMainView] Can't open empty main view");
        return;
    }
    ET_SendEventReturn(mainViewId, &ETUIViewStack::ET_pushView, mainView);
    if (!mainViewId.isValid()) {
        LogError("Can't cretae main view '%s'", mainView);
    }
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible) {
        ET_SendEvent(&ETSurface::ET_show);
    }
}