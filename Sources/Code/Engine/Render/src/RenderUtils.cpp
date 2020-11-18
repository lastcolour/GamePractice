#include "RenderUtils.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ImageBuffer.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"

#include <type_traits>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

namespace RenderUtils {

const char* GetGLError() {
    GLenum errCode = glGetError();
    if(errCode == GL_NO_ERROR) {
        return nullptr;
    }
    const char* errStr = "Unknown";
    switch(errCode) {
        case GL_INVALID_ENUM:
            errStr = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errStr = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_OPERATION:
            errStr = "GL_INVALID_ENUM";
            break;
        case GL_OUT_OF_MEMORY:
            errStr = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errStr = "GL_INVALID_ENUM";
            break;
        default:
            break;
    }
    return errStr;
}

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer) {
    auto size = framebuffer.texture.getSize();
    imageBuffer.setSizeAndClear(size);

    framebuffer.bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    if(auto errStr = GetGLError()) {
        LogError("[ReadFramebufferToImage] Can't read GL_COLOR_ATTACHMENT0 (Error: %s)", errStr);
        framebuffer.unbind();
        return false;
    }

    GLvoid* writePtr = static_cast<GLvoid*>(imageBuffer.getData().getWriteData());
    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, writePtr);

    if(auto errStr = GetGLError()) {
        LogError("[ReadFramebufferToImage] Can't read pixels from GL_COLOR_ATTACHMENT0 (Error: %s)", errStr);
        framebuffer.unbind();
        return false;
    }

    framebuffer.unbind();
    return true;
}

} // namespace RenderUtils