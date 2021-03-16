#ifndef __LOADING_SCREEN_MANAGER_HPP__
#define __LOADING_SCREEN_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETLoadingScreen.hpp"

class LoadingScreenManager : public SystemLogic,
    public ETNode<ETLoadingScreenManager> {
public:

    LoadingScreenManager();
    virtual ~LoadingScreenManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETLoadingScreenManager
    void ET_showLoadingScreen() override;
    void ET_hideLoadingScreen() override;
    bool ET_isLoadingScreenActive() const override;

private:

    bool loadingScreenActive;
    EntityId loadingViewId;
};

#endif /* __LOADING_SCREEN_MANAGER_HPP__ */