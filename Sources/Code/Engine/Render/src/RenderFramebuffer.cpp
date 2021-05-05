#include "RenderFramebuffer.hpp"
#include "Platform/OpenGL.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"

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

    color0.bind();
    if(!color0.resize(newSize)) {
        color0.unbind();
        return false;
    }
    color0.unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0.texId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(auto err = RenderUtils::GetGLError()) {
        LogError("[RenderFramebuffer::resize] Can't attach GL_COLOR_ATTACHMENT0 to a framebuffer (Error: %s)", err);
        return false;
    }

    if(type == EFramebufferType::Color_Depth_Stencil) {
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
        if(auto err = RenderUtils::GetGLError()) {
            LogError("[RenderFramebuffer::resize] Can't resize <depth, stencil> render buffer to a new size: [%d x %d] (Error: %s)",
                newSize.x, newSize.y, err);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            return false;
        }
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return true;
}