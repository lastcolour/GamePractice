#include "RenderFramebuffer.hpp"
#include "Platform/OpenGL.hpp"

RenderFramebuffer::RenderFramebuffer() :
    framebufferId(0) {
}

RenderFramebuffer::~RenderFramebuffer() {
}

void RenderFramebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
}

void RenderFramebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}