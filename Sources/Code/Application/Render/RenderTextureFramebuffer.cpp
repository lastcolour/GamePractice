#include "Render/RenderTextureFramebuffer.hpp"
#include "Logger.hpp"

#include <cassert>

RenderTextureFramebuffer::RenderTextureFramebuffer(GLsizei w, GLsizei h) :
    width(w),
    height(h),
    framebufferId(0),
    textureId(0) {
}

RenderTextureFramebuffer::~RenderTextureFramebuffer() {
    glDeleteTextures(1, &textureId);
    glDeleteFramebuffers(1, &framebufferId);
}

bool RenderTextureFramebuffer::init() {
    if(!GetEnv()->getRender()) {
        LogWarning("[RenderTextureFramebuffer] Cant init without render");
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

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

    textureBuffer.reset(new Color::Color_RGBA_Byte[width*height]);
    if(!textureBuffer) {
        auto bufferSize = width * height * sizeof(Color::Color_RGBA_Byte);
        LogError("[RenderTextureFramebuffer] Can't allocate bytes for read buffer %d", bufferSize);
        return false;
    }

    return glGetError() == GL_NO_ERROR;
}

int RenderTextureFramebuffer::getCompCount() const {
    return 4;
}

bool RenderTextureFramebuffer::bind() {
    if(!isBinded()) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    }
    return true;
}

bool RenderTextureFramebuffer::isBinded() const {
    if(framebufferId)
    {
        GLint currentReadFramebuffer = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentReadFramebuffer);
        bool res = framebufferId == currentReadFramebuffer;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentReadFramebuffer);
        res &= framebufferId == currentReadFramebuffer;
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
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texturePtr);

    auto errCode = glGetError();
    return errCode == GL_NO_ERROR;
}

void RenderTextureFramebuffer::clear() {
    if(!textureBuffer) {
        return;
    }
    for(GLsizei i=0; i< width * height; ++i) {
        textureBuffer[i] = Color::Color_RGBA_Byte(0, 0, 0);
    }
}

void RenderTextureFramebuffer::unbind() {
    if(isBinded()) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

GLsizei RenderTextureFramebuffer::getWidth() const {
    return width;
}

GLsizei RenderTextureFramebuffer::getHeight() const {
    return height;
}

ColorF RenderTextureFramebuffer::getColor(GLsizei w, GLsizei h) const {
    if(!textureBuffer) {
        return ColorF();
    }
    assert((w < width && h < height) && "Invalid access");
    auto offset = width * w + h;
    const auto& color = textureBuffer[offset];
    return ColorF(color.r / 255.f, color.g / 255.f, color.b / 255.f);
}

const void* RenderTextureFramebuffer::getPtr() const {
    return textureBuffer.get();
}