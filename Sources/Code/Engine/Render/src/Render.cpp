#include "Render.hpp"
#include "Render/RenderTextureFramebuffer.hpp"
#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderNode.hpp"
#include "Render/ETDebugRender.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Nodes/Node.hpp"

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
    ETNode<ETRenderUpdateTask>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    return true;
}

void Render::deinit() {
    ETNode<ETRender>::disconnect();
    ETNode<ETSurfaceEvents>::disconnect();
    ETNode<ETRenderUpdateTask>::disconnect();
    ETNode<ETRenderContextEvents>::disconnect();
}

void Render::ET_updateRender() {
    if(!canRenderToScreen()) {
        return;
    }
    tracker.onFrameStart();
    ET_SendEvent(&ETRenderNodeManager::ET_update);
    ET_SendEvent(&ETDebugRender::ET_update);
    ET_SendEvent(&ETSurface::ET_swapBuffers);
    tracker.onFrameEnd();
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

void Render::ET_drawFrameToFramebuffer(RenderTextureFramebuffer& renderFb, DrawContentFilter filter) {
    if(!canRenderToFramebuffer()) {
        return;
    }

    if(!renderFb.isValid()) {
        LogError("[Render::ET_drawFrameToFramebuffer] Can't draw to invalid framebuffer");
        return;
    }

    Vec2i prevViewPort(0);
    ET_SendEventReturn(prevViewPort, &ETRenderCamera::ET_getRenderPort);

    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, renderFb.getSize());

    renderFb.bind();

    if(filter != DrawContentFilter::NoDebugInfo) {
        tracker.onFrameStart();
    }
    ET_SendEvent(&ETRenderNodeManager::ET_update);
    if(filter != DrawContentFilter::NoDebugInfo) {
        ET_SendEvent(&ETDebugRender::ET_update);
        tracker.onFrameEnd();
    }

    renderFb.read();
    renderFb.unbind();

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
    ET_SendEvent(&ETDebugRender::ET_init);
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
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, size);
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

void Render::ET_updateParticles() {
    ET_SendEvent(&ETParticleEmitterProxyNode::ET_update, 0.01f);
}

void Render::ET_syncWithGame() {
    ET_PollAllEvents<ETRenderNodeManager>();
    ET_PollAllEvents<ETRenderProxyNode>();
    ET_PollAllEvents<ETParticleEmitterProxyNode>();
    ET_SendEvent(&ETRenderProxyNodeEvents::ET_syncWithRender);
}