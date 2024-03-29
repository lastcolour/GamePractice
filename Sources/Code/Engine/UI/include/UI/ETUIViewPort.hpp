#ifndef __ET_UI_VIEW_PORT_HPP__
#define __ET_UI_VIEW_PORT_HPP__

struct ETUIViewPort {
    virtual ~ETUIViewPort() = default;
    virtual const Vec2i& ET_getViewport() const = 0;
    virtual void ET_setViewPort(const Vec2i& newSize) = 0;
    virtual void ET_update() = 0;
};

struct ETUIViewPortEvents {
    virtual ~ETUIViewPortEvents() = default;
    virtual void ET_onViewPortChanged(const Vec2i& newSize) = 0;
};

#endif /* __ET_UI_VIEW_PORT_HPP__ */