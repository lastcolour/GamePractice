 #include "Render/Render.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Render/RenderTextureFramebuffer.hpp"
#include "Render/RenderContext.hpp"

#include "Platforms/OpenGL.hpp"

#include <type_traits>
#include <algorithm>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

Render::Render() :
    renderFb(nullptr),
    clearColor(0, 0, 0),
    hasContext(false),
    canOffscrenRender(false),
    canScreenRender(false),
    needUpdateRenderQueue(true) {
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
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    return true;
}

void Render::deinit() {
    ETNode<ETRender>::disconnect();
    ETNode<ETSurfaceEvents>::disconnect();
    ETNode<ETTimerEvents>::disconnect();
    ETNode<ETRenderContextEvents>::disconnect();
}

void Render::ET_onTick(float dt) {
    (void)dt;
    ET_drawFrame();
}

bool Render::ET_canRender() const {
    if(!hasContext) {
        return false;
    }
    if(renderFb && !canOffscrenRender) {
        return false;
    }
    if(!renderFb && !canScreenRender) {
        return false;
    }
    return true;
}

void Render::ET_updateRenderQueue() {
    needUpdateRenderQueue = true;
}

void Render::updateRenderQueue() {
    if(!needUpdateRenderQueue) {
        return;
    }
    needUpdateRenderQueue = false;
    renderQueue = ET_GetAll<ETRenderNode>();
    std::sort(renderQueue.begin(), renderQueue.end(), [](EntityId first, EntityId second){
        int firstPriority = 0;
        ET_SendEventReturn(firstPriority, first, &ETRenderNode::ET_getDrawPriority);
        int secondPriority = 0;
        ET_SendEventReturn(secondPriority, second, &ETRenderNode::ET_getDrawPriority);
        return firstPriority < secondPriority;
    });
}

void Render::ET_drawFrame() {
    if(!ET_canRender()) {
        return;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    auto clearColF = clearColor.getColorF();
    glClearColor(clearColF.r, clearColF.g, clearColF.b, clearColF.a);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderContext renderCtx;
    ET_SendEventReturn(renderCtx.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    updateRenderQueue();
    for(auto nodeId : renderQueue) {
        bool isVisible = false;
        ET_SendEventReturn(isVisible, nodeId, &ETRenderNode::ET_isVisible);
        bool isSrcMinusAlphaBlenRequired = false;
        if(!isVisible) {
            continue;
        }

        ET_SendEventReturn(isSrcMinusAlphaBlenRequired, nodeId, &ETRenderNode::ET_getScrMinusAlphaBlendFlag);
        renderCtx.setSrcMinusAlphaBlending(isSrcMinusAlphaBlenRequired);
        ET_SendEvent(nodeId, &ETRenderEvents::ET_onRender, renderCtx);
    }

    if(!renderFb) {
        ET_SendEvent(&ETSurface::ET_swapBuffers);
    }
}

const ColorB& Render::ET_getClearColor() const {
    return clearColor;
}

void Render::ET_setClearColor(const ColorB& col) {
    clearColor = col;
}

void Render::ET_setRenderToFramebuffer(RenderTextureFramebuffer* renderFramebuffer) {
    if(renderFb == renderFramebuffer) {
        return;
    } else if(renderFramebuffer == nullptr) {
        Vec2i size;
        ET_SendEventReturn(size, &ETSurface::ET_getSize);
        setViewport(size);
        renderFb = nullptr;
    } else {
        renderFb = renderFramebuffer;
        setViewport(renderFramebuffer->getSize());
    }
}

void Render::ET_onSurfaceDestroyed() {
    canOffscrenRender = false;
    canScreenRender = false;
}

void Render::ET_onSurfaceCreated() {
    canOffscrenRender = true;
    canScreenRender = false; 
    ET_SendEventReturn(canScreenRender, &ETSurface::ET_isVisible);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETSurface::ET_getSize);
    setViewport(renderPort);
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
    if(!renderFb) {
        setViewport(size);
    }
}

void Render::setViewport(const Vec2i& newViewport) {
    LogDebug("[Render::setViewport] Set viewport: [%ix%i]", newViewport.x, newViewport.y);
    glViewport(0, 0, newViewport.x, newViewport.y);
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, newViewport);
    ET_SendEvent(&ETRenderEvents::ET_onRenderPortResized);
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