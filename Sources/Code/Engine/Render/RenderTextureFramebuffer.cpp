#include "Render/RenderTextureFramebuffer.hpp"
#include "ETApplicationInterfaces.hpp"
#include "ETRenderInterfaces.hpp"
#include "Platforms/OpenGL.hpp"
#include "Render/RenderUtils.hpp"

#include <cassert>

RenderTextureFramebuffer::RenderTextureFramebuffer() :
    allocatedSize(0),
    size(0),
    framebufferId(0),
    textureId(0),
    textureBuffer() {
}

RenderTextureFramebuffer::~RenderTextureFramebuffer() {
    reset();
}

void RenderTextureFramebuffer::reset() {
    if(textureId) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
    if(framebufferId) {
        glDeleteFramebuffers(1, &framebufferId);
        framebufferId = 0;
    }
    size = Vec2i(0);
    allocatedSize = Vec2i(0);
}

bool RenderTextureFramebuffer::isValid() const {
    return framebufferId != 0 && textureId != 0;
}

bool RenderTextureFramebuffer::init() {
    GLContextType glCtxType = GLContextType::None;
    ET_SendEventReturn(glCtxType, &ETSurface::ET_getGLContextType);
    if(glCtxType == GLContextType::None) {
        LogWarning("[RenderTextureFramebuffer::init] Can't init render FB without GL context");
        return false;
    }
    if(framebufferId) {
        LogWarning("[RenderTextureFramebuffer::init] Double init");
        return false;
    }

    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    if(!framebufferId) {
        LogError("[RenderTextureFramebuffer::init] Can't create framebuffer");
        return false;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if(!textureId) {
        LogError("[RenderTextureFramebuffer::init] Can't create texture");
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    textureBuffer.reset(new Color::Color_RGBA_Byte[size.x * size.y]);
    if(!textureBuffer) {
        auto bufferSize = size.x * size.y * sizeof(Color::Color_RGBA_Byte);
        LogError("[RenderTextureFramebuffer::init] Can't allocate bytes for read buffer %d", bufferSize);
        return false;
    }

    if(!CheckGLError()) {
        LogError("[RenderTextureFramebuffer::init] Can't init framebuffer");
        return false;
    }
    return true;
}

bool RenderTextureFramebuffer::bind() {
    if(!isBinded()) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    }
    return true;
}

void RenderTextureFramebuffer::setSize(const Vec2i& newSize) {
    if(allocatedSize >= newSize) {
        size = newSize;
        return;
    }
    assert(newSize.x > 0 && newSize.y > 0 && "Invalid size of framebuffer");
    if(isValid()) {
        reset();
    }
    allocatedSize = newSize;
    size = newSize;
    if(!init()) {
        reset();
    }
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
        LogError("[RenderTextureFramebuffer::read] Can't read from an unbound framebuffer");
        return false;
    }

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    GLvoid* texturePtr = static_cast<GLvoid*>(&textureBuffer[0]);
    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, texturePtr);

    if(!CheckGLError()) {
        LogError("[RenderTextureFramebuffer::read] Can't read from framebuffer due to GL error");
        return false;
    }
    return true;
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