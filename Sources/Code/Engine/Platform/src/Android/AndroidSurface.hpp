#ifndef __ANDROID_SURFACE_HPP__
#define __ANDROID_SURFACE_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Platform/ETSurface.hpp"
#include "Platform/Android/ETAndroidInterfaces.hpp"

#include <EGL/egl.h>

class AndroidSurface : public SystemLogic,
    public ETNode<ETSurface>,
    public ETNode<ETAndroidActivityEvents>,
    public ETNode<ETInputUpdateTask> {
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
    bool ET_isVisible() const override;
    bool ET_canRender() const override;

    // ETAndroidActivityEvents
    void ET_onActivityEvent(ActivityEventType eventType) override;

    // ETInputUpdateTask
    void ET_updateInput() override;

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
    bool hasFocus;
};

#endif /* __ANDROID_SURFACE_HPP__ */