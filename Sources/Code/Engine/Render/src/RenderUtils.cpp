#include "RenderUtils.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ImageBuffer.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETLogger.hpp"
#include "Nodes/Node.hpp"
#include "RenderGraph/RenderContext.hpp"

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

void DrawWithMask(Node& node, RenderContext& ctx) {
    std::vector<Node*> drawNodes;
    Node* currNode = &node;
    drawNodes.push_back(currNode);
    while(true) {
        auto maskNode = currNode->getMaskNode();
        if(maskNode) {
            drawNodes.push_back(maskNode);
            currNode = maskNode;
        } else {
            break;
        }
    }
    std::reverse(drawNodes.begin(), drawNodes.end());

    glClear(GL_STENCIL_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_INCR, GL_INCR);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    for(int i = 0, sz = drawNodes.size() - 1; i < sz; ++i) {
        currNode = drawNodes[i];
        currNode->render(ctx);
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, drawNodes.size() - 1, 0xFF);

    currNode = drawNodes.back();
    currNode->render(ctx);

    glDisable(GL_STENCIL_TEST);
}

} // namespace RenderUtils