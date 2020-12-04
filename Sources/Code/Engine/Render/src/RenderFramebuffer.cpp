#include "RenderFramebuffer.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"
#include "Core/ETLogger.hpp"

RenderFramebuffer::RenderFramebuffer() :
    type(EFramebufferType::Color),
    framebufferId(0),
    renderBufferId(0) {
}

RenderFramebuffer::~RenderFramebuffer() {
}

void RenderFramebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
}

void RenderFramebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderFramebuffer::clear() {
    if(type == EFramebufferType::Color) {
        glClear(GL_COLOR_BUFFER_BIT);
    } else {
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

bool RenderFramebuffer::resize(const Vec2i& newSize) {
    if(color0.getSize() == newSize) {
        return true;
    }
    bool res = true;
    color0.bind();
    res = color0.resize(newSize);
    color0.unbind();
    if(!res) {
        return false;
    }
    if(type == EFramebufferType::Color_Depth_Stencil) {
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
        if(auto err = RenderUtils::GetGLError()) {
            LogError("[RenderFramebuffer::resize] Can't resize <depth, stencil> render buffer to a new size: %dx%d",
                newSize.x, newSize.y);
            res = false;
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    return res;
}