#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Core/ETPrimitives.hpp"
#include "Platform/ETSurface.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "RenderGraph/RenderGraph.hpp"

class Render : public SystemLogic,
    public ETNode<ETSurfaceEvents>,
    public ETNode<ETRender>,
    public ETNode<ETRenderUpdateTask>,
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
    void ET_registerNode(RenderNode* renderNode) override;

    // ETSurfaceEvents
    void ET_onSurfaceDestroyed() override;
    void ET_onSurfaceCreated() override;
    void ET_onSurfaceHidden() override;
    void ET_onSurfaceShown() override;
    void ET_onSurfaceLostFocus() override {}
    void ET_onSurfaceGainFocus() override {}
    void ET_onSurfaceResized(const Vec2i& size) override;

    // ETRenderUpdateTask
    void ET_updateRender() override;

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

    RenderGraph renderGraph;
    bool hasContext;
    bool canOffscrenRender;
    bool canScreenRender;
};

#endif /* __RENDER_HPP__ */