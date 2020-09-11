#ifndef __ET_LOADING_SCREEN_HPP__
#define __ET_LOADING_SCREEN_HPP__

struct ETLoadingScreenManager {
    virtual ~ETLoadingScreenManager() = default;
    virtual void ET_showLoadingScreen() = 0;
    virtual void ET_hideLoadingScreen() = 0;
    virtual bool ET_isLoadingScreenActive() const = 0;
};

struct ETLoadingScreenEvents {
    virtual ~ETLoadingScreenEvents() = default;
    virtual void ET_onLoadingScreenHidden() = 0;
};

#endif /* __ET_LOADING_SCREEN_HPP__ */