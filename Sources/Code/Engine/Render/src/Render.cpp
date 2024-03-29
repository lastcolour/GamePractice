#include "Render.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderNode.hpp"
#include "RenderUtils.hpp"
#include "Render/ImageBuffer.hpp"
#include "Commands/ETDrawCommands.hpp"
#include "Render/ETRenderTickManager.hpp"

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

void Render::ET_Render(float dt) {
    if(!canRenderToScreen()) {
        return;
    }

    ET_SendEvent(&ETRenderTickManager::ET_onRenderTick, dt);

    ET_SendEvent(&ETDrawCommandsManager::ET_renderToDefaultFBO);
    ET_SendEvent(&ETSurface::ET_swapBuffers);
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

void Render::ET_drawFrameToBuffer(ImageBuffer& imageBuffer, const Vec2i& drawSize, EDrawContentFilter filter) {
    imageBuffer.setSizeAndClear(drawSize);
    ET_drawFrameToBufferRaw(imageBuffer.getData().getWriteData(), drawSize, filter);
}

void Render::ET_drawFrameToBufferRaw(void* outBuffer, const Vec2i& drawSize, EDrawContentFilter filter) {
    if(!canRenderToFramebuffer()) {
        return;
    }

    Vec2i prevViewPort(0);
    ET_SendEventReturn(prevViewPort, &ETRenderCamera::ET_getRenderPort);
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, drawSize);

    ET_SendEvent(&ETDrawCommandsManager::ET_renderToBuffer, outBuffer, filter);

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

void Render::ET_PreRender() {
    if(canCreateRenderNodes()) {
        ET_PollAllEvents<ETDrawCommandsManager>();
        ET_SendEvent(&ETDrawCommandsManager::ET_preRender);
    }
}