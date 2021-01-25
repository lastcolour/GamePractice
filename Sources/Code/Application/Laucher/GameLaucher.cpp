#include "Laucher/GameLaucher.hpp"
#include "Laucher/GameLaucherConfig.hpp"
#include "Game/ETGameInterfaces.hpp"
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
    bool hasGLContent = false;
    ET_SendEventReturn(hasGLContent, &ETSurface::ET_hasOpenGLContext);
    if(hasGLContent) {
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
}

void GameLaucher::openMainView() {
    bool res = false;
    ET_SendEventReturn(res, &ETUIViewManager::ET_openView, UIViewType::Background);
    if(!res) {
        LogError("[GameLaucher::openMainView] Can't open background view");
        return;
    }
    ET_SendEventReturn(res, &ETUIViewManager::ET_openView, UIViewType::Main);
    if(!res) {
        LogError("[GameLaucher::openMainView] Can't open main view");
        return;
    }

    isViewExist = true;
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible) {
        ET_SendEvent(&ETSurface::ET_show);
    }
}