#include "LoadingScreenManager.hpp"
#include "UI/ETUIViewCache.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETLogger.hpp"

#include <limits>

LoadingScreenManager::LoadingScreenManager() :
    loadingScreenActive(false) {
}

LoadingScreenManager::~LoadingScreenManager() {
}

bool LoadingScreenManager::init() {
    ETNode<ETLoadingScreenManager>::connect(getEntityId());
    return true;
}

void LoadingScreenManager::deinit() {
}

bool LoadingScreenManager::ET_isLoadingScreenActive() const {
    return loadingScreenActive;
}

void LoadingScreenManager::ET_showLoadingScreen() {
    if(loadingScreenActive) {
        LogWarning("[LoadingScreenManager::ET_showLoadingScreen] Can't show loading screen while its already active");
        return;
    }
    if(!loadingViewId.isValid()) {
        ET_SendEventReturn(loadingViewId, &ETUIViewCache::ET_loadView, UIViewType::Loading);
        if(!loadingViewId.isValid()) {
            LogError("[LoadingScreenManager::ET_showLoadingScreen] Can't load loading view");
            return;
        }
    }

    LogDebug("[LoadingScreenManager::ET_showLoadingScreen] Show loading screen");
    loadingScreenActive = true;
    auto loadingViewZIndex = std::numeric_limits<int>::max() - 1000;
    ET_SendEvent(loadingViewId, &ETUIElement::ET_setZIndex, loadingViewZIndex);
    ET_SendEvent(&ETUIViewTransitionManager::ET_addAppearing, loadingViewId);
}

void LoadingScreenManager::ET_hideLoadingScreen() {
    if(!loadingScreenActive) {
        LogWarning("[LoadingScreenManager::ET_hideLoadingScreen] Can't hide non-active loading screen");
        return;
    }

    LogDebug("[LoadingScreenManager::ET_hideLoadingScreen] Hide loading screen");
    loadingScreenActive = false;
    ET_SendEvent(&ETUIViewTransitionManager::ET_addDisappearing, loadingViewId);
}