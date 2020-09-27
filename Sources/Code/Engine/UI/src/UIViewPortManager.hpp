#ifndef __UI_VIEW_PORT_MANAGER_HPP__
#define __UI_VIEW_PORT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Render/ETRenderCamera.hpp"

class UIViewPortManager : public SystemLogic, 
    public ETNode<ETUIViewPort>,
    public ETNode<ETRenderCameraEvents> {
public:

    UIViewPortManager();
    virtual ~UIViewPortManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIViewPort
    const Vec2i& ET_getViewport() const override;
    void ET_setViewPort(const Vec2i& newSize) override;

    // ETRenderCameraEvents
    void ET_onRenderPortResized(const Vec2i& newSize) override;

private:

    Vec2i viewPort;
    bool viewPortUpdated;
};

#endif /* __UI_VIEW_PORT_MANAGER_HPP__ */