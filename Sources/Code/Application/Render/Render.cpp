#include "Render/Render.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Render/RenderTextureFramebuffer.hpp"

#include "Platforms/OpenGL.hpp"

#include <algorithm>
#include <type_traits>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

Render::Render() :
    renderFb(nullptr),
    clearColor(0, 0, 0) {
}

 Render::~Render() {
 }

bool Render::init() {
    GLContextType glCtxType = GLContextType::None;
    ET_SendEventReturn(glCtxType, &ETSurface::ET_getGLContextType);
    if(glCtxType == GLContextType::None) {
        LogError("[Render::onInit] Can't init render without GL context");
        return false;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    ETNode<ETRender>::connect(getEntityId());
    ETNode<ETSurfaceEvents>::connect(getEntityId());
    ETNode<ETTimerEvents>::connect(getEntityId());

    return true;
}

void Render::deinit() {
    ETNode<ETRender>::disconnect();
    ETNode<ETSurfaceEvents>::disconnect();
    ETNode<ETTimerEvents>::disconnect();
}

void Render::ET_onTick(float dt) {
    (void)dt;
    ET_drawFrame();
}

void Render::ET_drawFrame() {
    bool isVisible = false;
    ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
    if(!isVisible && !renderFb) {
        return;
    }

    auto clearColF = clearColor.getColorF();
    glClearColor(clearColF.r, clearColF.g, clearColF.b, clearColF.a);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderContext renderCtx;
    ET_SendEventReturn(renderCtx.proj2dMat, &ETRenderCamera::ET_getProj2DMat4);

    for(auto entId : ET_GetAll<ETRenderSimpleLogic>()) {
        ET_SendEvent(entId, &ETRenderEvents::ET_onRender, renderCtx);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(auto entId : ET_GetAll<ETRenderTextLogic>()) {
        ET_SendEvent(entId, &ETRenderEvents::ET_onRender, renderCtx);
    }

    glDisable(GL_BLEND);

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

void Render::ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) {
    (void)touchType;
    (void)pt;
}

void Render::ET_onSurfaceResize(const Vec2i& size) {
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