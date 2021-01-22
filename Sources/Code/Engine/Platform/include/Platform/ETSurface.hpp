#ifndef __ET_SURFACE_HPP__
#define __ET_SURFACE_HPP__

#include "Math/Vector.hpp"

struct ETSurface {
    virtual ~ETSurface() = default;
    virtual bool ET_show() = 0;
    virtual bool ET_hide() = 0;
    virtual void ET_close() = 0;
    virtual Vec2i ET_getSize() const = 0;
    virtual void ET_swapBuffers() = 0;
    virtual bool ET_isVisible() const = 0;
    virtual bool ET_hasOpenGLContext() const = 0;
};

enum class EActionType {
    Press = 0,
    Move,
    Release
};

enum class EButtonId {
    Back = 0,
};

struct ETInputEvents {
    virtual void ET_onTouch(EActionType actionType, const Vec2i& pt) = 0;
    virtual void ET_onButton(EActionType actionType, EButtonId buttonId) = 0;
};

struct ETSurfaceEvents {
    virtual ~ETSurfaceEvents() = default;
    virtual void ET_onSurfaceCreated() = 0;
    virtual void ET_onSurfaceDestroyed() = 0;
    virtual void ET_onSurfaceHidden() = 0;
    virtual void ET_onSurfaceShown() = 0;
    virtual void ET_onSurfaceLostFocus() = 0;
    virtual void ET_onSurfaceGainFocus() = 0;
    virtual void ET_onSurfaceResized(const Vec2i& size) = 0;
};

#endif /* __ET_SURFACE_HPP__ */