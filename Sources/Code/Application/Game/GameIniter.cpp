#include "Game/GameIniter.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

GameIniter::GameIniter() :
    isViewExist(false) {
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
    if(!isViewExist) {
        openMainView();
    }
}

void GameIniter::ET_onSurfaceDestroyed() {
    isViewExist = true;
}

void GameIniter::openMainView() {
    const char* mainView = nullptr;
    ET_SendEventReturn(mainView, &ETGameConfig::ET_getMainViewName);
    if(!mainView || !mainView[0]) {
        LogError("[GameIniter::openMainView] Can't open empty main view");
        return;
    }
    ET_SendEvent(&ETUIViewStack::ET_pushView, mainView);
    EntityId mainViewId;
    ET_SendEventReturn(mainViewId, &ETUIViewStack::ET_getActiveViewId);
    if (!mainViewId.isValid()) {
        LogError("Can't cretae main view '%s'", mainView);
    }
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible) {
        ET_SendEvent(&ETSurface::ET_show);
    }
    isViewExist = true;
}