#ifndef __ANDROID_FULLSCREEN_SETUP_HPP__
#define __ANDROID_FULLSCREEN_SETUP_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"

class AndroidFullscreenSetup : public SystemLogic,
    public ETNode<ETAndroidActivityEvents> {
public:

    AndroidFullscreenSetup();
    virtual ~AndroidFullscreenSetup();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAndroidActivityEvents
    void ET_onActivityEvent(ActivityEventType eventType) override;

private:

    void makeAppFullScreen();
};

#endif /* __ANDROID_FULLSCREEN_SETUP_HPP__ */