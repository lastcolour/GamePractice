#include "Laucher/GameLaucher.hpp"
#include "Laucher/GameLaucherConfig.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETApplication.hpp"

#include <cassert>

GameLaucher::GameLaucher() :
    isViewExist(false) {
}

GameLaucher::~GameLaucher() {
}

bool GameLaucher::init() {
    bool isSurfaceValid = false;
    ET_SendEventReturn(isSurfaceValid, &ETSurface::ET_isValid);
    if(isSurfaceValid) {
        openMainView();
    }
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    return true;
}

void GameLaucher::deinit() {
    ETNode<ETSurfaceEvents>::disconnect();
}

void GameLaucher::ET_onSurfaceCreated() {
    if(!isViewExist) {
        openMainView();
    }
}

void GameLaucher::ET_onSurfaceDestroyed() {
    isViewExist = true;
}

void GameLaucher::openMainView() {
    GameLaucherConfig* laucherConfig = ET_getConfig<GameLaucherConfig>();
    if(!laucherConfig) {
        assert(false && "Invalid configs");
        return;
    }

    if(laucherConfig->mainView.empty()) {
        LogError("[GameIniter::openMainView] Can't open empty main view");
        return;
    }

    ET_SendEvent(&ETUIViewStack::ET_pushView, laucherConfig->mainView.c_str());
    EntityId mainViewId;
    ET_SendEventReturn(mainViewId, &ETUIViewStack::ET_getActiveViewId);
    if(!mainViewId.isValid()) {
        LogError("[GameIniter::openMainView] Can't cretae main view '%s'", laucherConfig->mainView);
    }
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible) {
        ET_SendEvent(&ETSurface::ET_show);
    }
    isViewExist = true;
}