#include "Initer/GameIniter.hpp"
#include "Initer/GameIniterConfig.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>

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
    GameIniterConfig* initConfig = ET_getConfig<GameIniterConfig>();
    if(!initConfig) {
        assert(false && "Invalid configs");
        return;
    }

    if(!initConfig->background.empty()) {
        EntityId backgroundId;
        ET_SendEventReturn(backgroundId, &ETEntityManager::ET_createEntity, initConfig->background.c_str());
        if(!backgroundId.isValid()) {
            LogError("[GameIniter::openMainView] Can't create background: %s", initConfig->background);
        }
    }

    if(initConfig->mainView.empty()) {
        LogError("[GameIniter::openMainView] Can't open empty main view");
        return;
    }

    ET_SendEvent(&ETUIViewStack::ET_pushView, initConfig->mainView.c_str());
    EntityId mainViewId;
    ET_SendEventReturn(mainViewId, &ETUIViewStack::ET_getActiveViewId);
    if (!mainViewId.isValid()) {
        LogError("[GameIniter::openMainView] Can't cretae main view '%s'", initConfig->mainView);
    }
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible) {
        ET_SendEvent(&ETSurface::ET_show);
    }
    isViewExist = true;
}