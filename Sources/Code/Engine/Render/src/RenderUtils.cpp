#include "RenderUtils.hpp"

#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"

bool CheckGLError(const char* label) {
    GLenum errCode = glGetError();
    if(errCode != GL_NO_ERROR) {
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
        LogWarning("[CheckGLError] Error: %s; Label: %s", errStr, std::string(label));
        return false;
    }
    return true;
}