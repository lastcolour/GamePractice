#include "Render/RenderTextureFramebuffer.hpp"
#include "ETApplicationInterfaces.hpp"
#include "ETRenderInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <cassert>

RenderTextureFramebuffer::RenderTextureFramebuffer(int w, int h) :
    size(w, h),
    framebufferId(0),
    textureId(0),
    textureBuffer() {
}

RenderTextureFramebuffer::~RenderTextureFramebuffer() {
    glDeleteTextures(1, &textureId);
    glDeleteFramebuffers(1, &framebufferId);

    ET_SendEvent(&ETRender::ET_setRenderToFramebuffer, nullptr);
}

bool RenderTextureFramebuffer::init() {
    GLContextType glCtxType = GLContextType::None;
    ET_SendEventReturn(glCtxType, &ETSurface::ET_getGLContextType);
    if(glCtxType == GLContextType::None) {
        LogWarning("[RenderTextureFramebuffer::init] Can't init render FB without GL context");
        return false;
    }
    if(framebufferId) {
        LogWarning("[RenderTextureFramebuffer] Double init");
        return false;
    }

    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    if(!framebufferId) {
        LogError("[RenderTextureFramebuffer] Can't create framebuffer");
        return false;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if(!textureId) {
        LogError("[RenderTextureFramebuffer] Can't create texture");
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    textureBuffer.reset(new Color::Color_RGBA_Byte[size.x * size.y]);
    if(!textureBuffer) {
        auto bufferSize = size.x * size.y * sizeof(Color::Color_RGBA_Byte);
        LogError("[RenderTextureFramebuffer] Can't allocate bytes for read buffer %d", bufferSize);
        return false;
    }

    return glGetError() == GL_NO_ERROR;
}

bool RenderTextureFramebuffer::bind() {
    if(!isBinded()) {
        ET_SendEvent(&ETRender::ET_setRenderToFramebuffer, this);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    }
    return true;
}

Vec2i RenderTextureFramebuffer::getSize() const {
    return size;
}

bool RenderTextureFramebuffer::isBinded() const {
    if(framebufferId)
    {
        GLint currentFb = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentFb);
        bool res = framebufferId == static_cast<GLuint>(currentFb);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFb);
        res &= framebufferId == static_cast<GLuint>(currentFb);
        return res;
    }
    return false;
}

bool RenderTextureFramebuffer::read() {
    if(!isBinded()) {
        return false;
    }
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    GLvoid* texturePtr = static_cast<GLvoid*>(&textureBuffer[0]);
    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, texturePtr);

    auto errCode = glGetError();
    return errCode == GL_NO_ERROR;
}

void RenderTextureFramebuffer::clear() {
    if(!textureBuffer) {
        return;
    }
    for(int i=0; i < size.x * size.y; ++i) {
        textureBuffer[i] = Color::Color_RGBA_Byte(0, 0, 0);
    }
}

void RenderTextureFramebuffer::unbind() {
    if(isBinded()) {
        ET_SendEvent(&ETRender::ET_setRenderToFramebuffer, nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

ColorB RenderTextureFramebuffer::getColor(int w, int h) const {
    if(!textureBuffer) {
        return ColorB();
    }
    assert((w < size.x && h < size.y) && "Invalid access to pixel");
    auto offset = size.x * h + w;
    return textureBuffer[offset];
}

const void* RenderTextureFramebuffer::getPtr() const {
    return textureBuffer.get();
}