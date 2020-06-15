#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTimer.hpp"
#include "Core/ETSurface.hpp"
#include "Render/ETRenderInterfaces.hpp"

class Render : public SystemLogic,
    public ETNode<ETSurfaceEvents>,
    public ETNode<ETRender>,
    public ETNode<ETSystemTimerEvents>,
    public ETNode<ETRenderContextEvents> {
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
    void ET_drawFrameToFramebufer(RenderTextureFramebuffer& renderFb) override;
    void ET_updateRenderQueue() override;

    // ETSurfaceEvents
    void ET_onSurfaceDestroyed() override;
    void ET_onSurfaceCreated() override;
    void ET_onSurfaceHidden() override;
    void ET_onSurfaceShown() override;
    void ET_onSurfaceLostFocus() override {}
    void ET_onSurfaceGainFocus() override {}
    void ET_onSurfaceResized(const Vec2i& size) override;

    // ETSystemTimerEvents
    void ET_onSystemTick(float dt) override;

    // ETRenderContextEvents
    void ET_onContextSuspended() override;
    void ET_onContextRestored() override;
    void ET_onContextReCreated() override;

private:

    bool canRenderToScreen() const;
    bool canRenderToFramebuffer() const;
    void updateRenderQueue();
    void updateRenderPort(const Vec2i& size);
    void drawRoutine();

private:

    std::vector<EntityId> renderQueue;
    ColorB clearColor;
    bool hasContext;
    bool canOffscrenRender;
    bool canScreenRender;
    bool needUpdateRenderQueue;
};

#endif /* __RENDER_HPP__ */