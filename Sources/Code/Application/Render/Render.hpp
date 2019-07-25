#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

class Render : public SystemLogic,
    public ETNode<ETSurfaceEvents>,
    public ETNode<ETRender>,
    public ETNode<ETTimerEvents> {
public:

    Render();
    virtual ~Render();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRender
    const ColorB& ET_getClearColor() const override;
    void ET_setClearColor(const ColorB& col) override;
    void ET_drawFrame() override;
    void ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFb) override;

    // ETSurfaceEvents
    void ET_onSurfaceResize(const Vec2i& size) override;
    void ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

private:

    void setViewport(const Vec2i& size);

private:

    RenderTextureFramebuffer* renderFb;
    ColorB clearColor;
};

#endif /* __RENDER_HPP__ */