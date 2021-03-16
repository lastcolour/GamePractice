#include "Render.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderNode.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Nodes/Node.hpp"
#include "RenderUtils.hpp"

Render::Render() :
    canOffscrenRender(false),
    canScreenRender(false) {
}

Render::~Render() {
}

bool Render::init() {
    if(RenderUtils::IsOpenGLContextExists()) {
        ET_onSurfaceCreated();
    }

    ETNode<ETRender>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    ETNode<ETRenderUpdateTask>::connect(getEntityId());

    return true;
}

void Render::deinit() {
    ETNode<ETRender>::disconnect();
    ETNode<ETSurfaceEvents>::disconnect();
    ETNode<ETRenderUpdateTask>::disconnect();
}

void Render::ET_updateRender(float dt) {
    if(!canRenderToScreen()) {
        return;
    }
    tracker.onFrameStart();
    ET_SendEvent(&ETRenderNodeManager::ET_drawFrame);
    ET_SendEvent(&ETSurface::ET_swapBuffers);
    tracker.onFrameEnd();
}

void Render::ET_updateParticles(float dt) {
    ET_SendEvent(&ETParticlesUpdate::ET_updateEmitter, dt);
}

bool Render::canRenderToScreen() const {
    if(!canScreenRender) {
        return false;
    }
    return true;
}

bool Render::canRenderToFramebuffer() const {
    if(!canOffscrenRender) {
        return false;
    }
    return true;
}

void Render::ET_drawFrameToBuffer(ImageBuffer& imageBuffer, const Vec2i& drawSize, DrawContentFilter filter) {
    if(!canRenderToFramebuffer()) {
        return;
    }

    Vec2i prevViewPort(0);
    ET_SendEventReturn(prevViewPort, &ETRenderCamera::ET_getRenderPort);
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, drawSize);

    if(filter != DrawContentFilter::NoDebugInfo) {
        tracker.onFrameStart();
    }

    ET_SendEvent(&ETRenderNodeManager::ET_drawFrameToBuffer, imageBuffer, filter);

    if(filter != DrawContentFilter::NoDebugInfo) {
        tracker.onFrameEnd();
    }

    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, prevViewPort);
}

void Render::ET_onSurfaceDestroyed() {
    canOffscrenRender = false;
    canScreenRender = false;
}

void Render::ET_onSurfaceCreated() {
    canOffscrenRender = true;
    canScreenRender = false;
    ET_SendEventReturn(canScreenRender, &ETSurface::ET_isVisible);
}

void Render::ET_onSurfaceHidden() {
    canOffscrenRender = true;
    canScreenRender = false;
}

void Render::ET_onSurfaceShown() {
    canOffscrenRender = true;
    canScreenRender = true;
}

bool Render::canCreateRenderNodes() const {
    return canOffscrenRender;
}

void Render::ET_onSurfaceResized(const Vec2i& size) {
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, size);
}

void Render::ET_syncWithGame() {
    if(canCreateRenderNodes()) {
        ET_PollAllEvents<ETRenderNodeManager>();
    }
    ET_SendEvent(&ETRenderProxyNodeEvents::ET_syncWithRender);
}