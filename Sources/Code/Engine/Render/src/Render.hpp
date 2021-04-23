#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETTasks.hpp"
#include "Platform/ETSurface.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "RenderGraph/RenderGraph.hpp"

class Render : public SystemLogic,
    public ETNode<ETSurfaceEvents>,
    public ETNode<ETRender>,
    public ETNode<ETRenderUpdateTask> {
public:

    Render();
    virtual ~Render();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRender
    void ET_drawFrameToBuffer(ImageBuffer& imageBuffer, const Vec2i& drawSize, DrawContentFilter filter) override;
    void ET_drawFrameToBufferRaw(void* outBuffer, const Vec2i& drawSize, DrawContentFilter filter) override;

    // ETSurfaceEvents
    void ET_onSurfaceDestroyed() override;
    void ET_onSurfaceCreated() override;
    void ET_onSurfaceHidden() override;
    void ET_onSurfaceShown() override;
    void ET_onSurfaceLostFocus() override {}
    void ET_onSurfaceGainFocus() override {}
    void ET_onSurfaceResized(const Vec2i& size) override;

    // ETRenderUpdateTask
    void ET_updateRender(float dt) override;
    void ET_updateParticles(float dt) override;
    void ET_syncWithGame() override;

private:

    bool canRenderToScreen() const;
    bool canRenderToFramebuffer() const;
    bool canCreateRenderNodes() const;

private:

    bool canOffscrenRender;
    bool canScreenRender;
};

#endif /* __RENDER_HPP__ */