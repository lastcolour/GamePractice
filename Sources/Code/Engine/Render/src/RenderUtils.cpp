#include "RenderUtils.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ImageBuffer.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"
#include "Nodes/Node.hpp"
#include "RenderGraph/RenderContext.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Platform/ETSurface.hpp"

#include <type_traits>
#include <cassert>

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
    auto size = framebuffer.color0.getSize();
    imageBuffer.setSizeAndClear(size);

    framebuffer.bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    if(auto errStr = GetGLError()) {
        LogError("[ReadFramebufferToImage6] Can't read GL_COLOR_ATTACHMENT0 (Error: %s)", errStr);
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

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fromFBO.framebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, toFBO.framebufferId);

    Vec2i fromSize = fromFBO.color0.getSize();
    Vec2i toSize = toFBO.color0.getSize();
    glBlitFramebuffer(0, 0, fromSize.x, fromSize.y, 0, 0, toSize.x, toSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fromFBO.framebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    Vec2i size = fromFBO.color0.getSize();
    glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

bool IsOpenGLContextExists() {
    bool hasContext = false;
    ET_SendEventReturn(hasContext, &ETSurface::ET_hasOpenGLContext);
    return hasContext;
}

} // namespace RenderUtils