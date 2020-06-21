#ifndef __ANDROID_SURFACE_HPP__
#define __ANDROID_SURFACE_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"

#include <EGL/egl.h>

class AndroidSurface : public SystemLogic,
    public ETNode<ETSurface>,
    public ETNode<ETAndroidActivityEvents>,
    public ETNode<ETSystemTimerEvents> {
public:
    AndroidSurface();
    virtual ~AndroidSurface();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSurface
    bool ET_show() override;
    bool ET_hide() override;
    void ET_close() override;
    Vec2i ET_getSize() const override;
    void ET_swapBuffers() override;
    GLContextType ET_getGLContextType() const override;
    bool ET_isVisible() const override;
    bool ET_isValid() const override;

    // ETAndroidActivityEvents
    void ET_onActivityEvent(ActivityEventType eventType) override;

    // ETSystemTimerEvents
    void ET_onSystemTick(float dt) override;

private:

    void onNativeWindowCreated();
    void onNativeWindowDestoryed();

    bool createEGLSurface();
    bool createEGLDisplay();
    bool createEGLContext();

private:

    Vec2i size;
    ANativeWindow* nativeWindow;
    EGLContext context;
    EGLSurface surface;
    EGLDisplay display;
    EGLConfig config;
};

#endif /* __ANDROID_SURFACE_HPP__ */