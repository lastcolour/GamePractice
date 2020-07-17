#include "Render.hpp"
#include "Render/RenderTextureFramebuffer.hpp"
#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderNode.hpp"

#include <type_traits>
#include <algorithm>
#include <cassert>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

Render::Render() :
    hasContext(false),
    canOffscrenRender(false),
    canScreenRender(false) {
}

 Render::~Render() {
 }

bool Render::init() {
    GLContextType contextType = GLContextType::None;
    ET_SendEventReturn(contextType, &ETSurface::ET_getGLContextType);
    if(contextType != GLContextType::None) {
        hasContext = true;
        ET_SendEventReturn(canOffscrenRender, &ETSurface::ET_isValid);
        if(canOffscrenRender) {
            ET_onSurfaceCreated();
        }
    }

    ETNode<ETRender>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    ETNode<ETSystemTimerEvents>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    return true;
}

void Render::deinit() {
    ETNode<ETRender>::disconnect();
    ETNode<ETSurfaceEvents>::disconnect();
    ETNode<ETSystemTimerEvents>::disconnect();
    ETNode<ETRenderContextEvents>::disconnect();
}

void Render::ET_onSystemTick(float dt) {
    (void)dt;
    ET_drawFrame();
}

bool Render::canRenderToScreen() const {
    if(!hasContext) {
        return false;
    }
    if(!canScreenRender) {
        return false;
    }
    return true;
}

bool Render::canRenderToFramebuffer() const {
    if(!hasContext) {
        return false;
    }
    if(!canOffscrenRender) {
        return false;
    }
    return true;
}

void Render::ET_drawFrame() {
    if(!canRenderToScreen()) {
        return;
    }

    Vec2i surfaceSize(0);
    ET_SendEventReturn(surfaceSize, &ETSurface::ET_getSize);
    updateRenderPort(surfaceSize);

    renderGraph.render();

    ET_SendEvent(&ETSurface::ET_swapBuffers);
}

void Render::ET_drawFrameToFramebufer(RenderTextureFramebuffer& renderFb) {
    if(!canRenderToFramebuffer()) {
        return;
    }

    if(!renderFb.isValid()) {
        LogError("[Render::ET_drawFrameToFramebufer] Can't draw to invalid framebuffer");
        return;
    }

    updateRenderPort(renderFb.getSize());

    renderFb.bind();

    renderGraph.render();

    renderFb.read();
    renderFb.unbind();
}

void Render::updateRenderPort(const Vec2i& size) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    if(renderPort == size) {
        return;
    }
    LogDebug("[Render::updateRenderPort] Set viewport: [%ix%i]", size.x, size.y);
    glViewport(0, 0, size.x, size.y);
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, size);
    ET_SendEvent(&ETRenderCameraEvents::ET_onRenderPortResized);
}

const ColorB& Render::ET_getClearColor() const {
    return renderGraph.getContext().getClearColor();
}

void Render::ET_setClearColor(const ColorB& col) {
    renderGraph.getContext().setClearColor(col);
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

void Render::ET_onSurfaceResized(const Vec2i& size) {
    (void)size;
}

void Render::ET_onContextReCreated() {
    hasContext = true;
    ET_SendEvent(&ETRenderResourceManager::ET_forgetResoruces);
}

void Render::ET_onContextSuspended() {
    hasContext = false;
}

void Render::ET_onContextRestored() {
    hasContext = true;
}

void Render::ET_registerNode(RenderNode* renderNode) {
    assert(renderNode && "Invalid render node");
    renderGraph.addChild(renderNode);
}