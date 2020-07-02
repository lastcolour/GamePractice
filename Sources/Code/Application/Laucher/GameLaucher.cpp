#include "Laucher/GameLaucher.hpp"
#include "Laucher/GameLaucherConfig.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETApplication.hpp"
#include "UI/ETUIView.hpp"

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
    isViewExist = false;
}

void GameLaucher::openMainView() {
    auto laucherConfig = ET_getShared<GameLaucherConfig>();
    if(!laucherConfig) {
        assert(false && "Invalid configs");
        return;
    }

    if(laucherConfig->mainView.empty()) {
        LogError("[GameIniter::openMainView] Main view is empty");
        return;
    }

    EntityId mainViewId;
    ET_SendEventReturn(mainViewId, &ETEntityManager::ET_createEntity, laucherConfig->mainView.c_str());
    if(!mainViewId.isValid()) {
        LogError("[GameLaucher::openMainView] Can't open main view: '%s'", laucherConfig->mainView);
        return;
    }

    isViewExist = true;
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible) {
        ET_SendEvent(&ETSurface::ET_show);
    }
}