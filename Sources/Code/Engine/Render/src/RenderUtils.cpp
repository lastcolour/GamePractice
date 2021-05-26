#include "RenderUtils.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ImageBuffer.hpp"
#include "Render/ETRenderManager.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderGraph/RenderContext.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Platform/ETSurface.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "Nodes/ImageNode.hpp"
#include "Nodes/NinePatchNode.hpp"
#include "Nodes/SimpleNode.hpp"
#include "Nodes/TextNode.hpp"
#include "Nodes/GradientNode.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Nodes/BlurNode.hpp"
#include "RenderTexture.hpp"
#include "Render/ParticlesEmitterConfig.hpp"

#include <type_traits>
#include <cassert>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

namespace RenderUtils {

const char* GetGLError() {
    GLenum errCode = glGetError();
    const char* errStr = nullptr;
    switch(errCode) {
        case GL_NO_ERROR: {
            break;
        }
        case GL_INVALID_ENUM: {
            errStr = "GL_INVALID_ENUM";
            break;
        }
        case GL_INVALID_VALUE: {
            errStr = "GL_INVALID_VALUE";
            break;
        }
        case GL_INVALID_OPERATION: {
            errStr = "GL_INVALID_OPERATION";
            break;
        }
        case GL_OUT_OF_MEMORY: {
            errStr = "GL_OUT_OF_MEMORY";
            break;
        }
        case GL_INVALID_FRAMEBUFFER_OPERATION: {
            errStr = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        default: {
            errStr = "Unknown";
            break;
        }
    }
    return errStr;
}

const char* GetFBOError(GLenum framebufferType) {
    GLenum errCode = glCheckFramebufferStatus(framebufferType);
    const char* errStr = nullptr;
    switch(errCode) {
        case GL_FRAMEBUFFER_COMPLETE: {
            break;
        }
        case GL_FRAMEBUFFER_UNDEFINED: {
            errStr = "GL_FRAMEBUFFER_UNDEFINED";
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
            errStr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
            errStr = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED: {
            errStr = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
            errStr = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        }
        default: {
            errStr = "Uknown";
            break;
        }
    }
    return errStr;
}

bool ReadFramebufferToBuffer(RenderFramebuffer& framebuffer, void* out) {
    auto size = framebuffer.color0.getSize();

    framebuffer.bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    if(auto errStr = GetGLError()) {
        LogError("[ReadFramebufferToBuffer] Can't read GL_COLOR_ATTACHMENT0 (Error: %s)", errStr);
        framebuffer.unbind();
        return false;
    }

    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, out);

    if(auto errStr = GetGLError()) {
        LogError("[ReadFramebufferToBuffer] Can't read pixels from GL_COLOR_ATTACHMENT0 (Error: %s)", errStr);
        framebuffer.unbind();
        return false;
    }

    framebuffer.unbind();
    return true;
}

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer) {
    auto size = framebuffer.color0.getSize();
    imageBuffer.setSizeAndClear(size);

    return ReadFramebufferToBuffer(framebuffer, imageBuffer.getData().getWriteData());
}

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fromFBO.framebufferId);
    if(auto err = GetFBOError(GL_READ_FRAMEBUFFER)) {
        LogError("[BlitFromFBOtoFBO] Invalid from FBO (Error: %s)", err);
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, toFBO.framebufferId);
    if(auto err = GetFBOError(GL_DRAW_FRAMEBUFFER)) {
        LogError("[BlitFromFBOtoFBO] Invalid to FBO (Error: %s)", err);
        return;
    }

    Vec2i fromSize = fromFBO.color0.getSize();
    Vec2i toSize = toFBO.color0.getSize();
    glBlitFramebuffer(0, 0, fromSize.x, fromSize.y, 0, 0, toSize.x, toSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    if(auto err = GetGLError()) {
        LogError("[BlitFromFBOtoFBO] Can't blit one FBO to another FBO (Error: %s)", err);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fromFBO.framebufferId);
    if(auto err = GetFBOError(GL_READ_FRAMEBUFFER)) {
        LogError("[BlitFromFBOtoDefaultFBO] Invalid from FBO (Error: %s)", err);
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    Vec2i size = fromFBO.color0.getSize();
    glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    if(auto err = GetGLError()) {
        LogError("[BlitFromFBOtoDefaultFBO] Can't blit one FBO to default FBO (Error: %s)", err);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

bool IsOpenGLContextExists() {
    bool hasContext = false;
    ET_SendEventReturn(hasContext, &ETSurface::ET_canRender);
    return hasContext;
}

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale) {
    auto resMat = tm.toMat4();
    Math::AddScale(resMat, scale / 2.f);
    return resMat;
}

std::unique_ptr<Node> CreateRenderNode(RenderNodeType nodeType) {
    std::unique_ptr<Node> node;
    switch(nodeType) {
        case RenderNodeType::Quad: {
            break;
        }
        case RenderNodeType::NinePatchImage: {
            node.reset(new NinePatchNode());
            break;
        }
        case RenderNodeType::Gradient: {
            node.reset(new GradientNode());
            break;
        }
        case RenderNodeType::Image: {
            node.reset(new ImageNode());
            break;
        }
        case RenderNodeType::Simple: {
            node.reset(new SimpleNode());
            break;
        }
        case RenderNodeType::Text: {
            node.reset(new TextNode());
            break;
        }
        case RenderNodeType::ParticleEmmiter: {
            node.reset(new ParticlesNode());
            break;
        }
        case RenderNodeType::Blur:
            node.reset(new BlurNode());
            break;
        default: {
            assert(false && "Invalid node type");
            return nullptr;
        }
    }
    return node;
}

std::shared_ptr<RenderTexture> CreateTexture(const TextureInfo& texInfo, ETextureDataType texType) {
    std::shared_ptr<RenderTexture> texObj;
    ET_SendEventReturn(texObj, &ETRenderTextureManager::ET_createFromFile,
        texInfo.filename.c_str(), texType);
    return texObj;
}

void ApplyTextureInfo(RenderTexture& texObj, const TextureInfo& texInfo) {
    if(texObj.minLerpType == texInfo.lerpType && texObj.magLerpType == texInfo.lerpType) {
        return;
    }
    texObj.bind();
    texObj.setLerpType(texInfo.lerpType, texInfo.lerpType);
    texObj.unbind();
}

Vec2 GetNinePatchVertexCoord(const Vec2i& imageSize, const Vec2& drawSize, const Vec2& patches, float patchScale) {
    Vec2 scale(imageSize.x / drawSize.x , imageSize.y / drawSize.y);

    float r = scale.x / scale.y;

    float hPatch_orig = patches.x * scale.x;
    float hPatch_scaled = patches.x * r;
    float hPatch = Math::Lerp(hPatch_orig, hPatch_scaled, patchScale);
    hPatch = 2.f * std::min(hPatch, 0.4999f);

    float vPatch_orig = patches.y * scale.y;
    float vPatch_scaled = patches.y;
    float vPatch = Math::Lerp(vPatch_orig, vPatch_scaled, patchScale);
    vPatch = 2.f * std::min(vPatch, 0.4999f);

    return Vec2(hPatch, vPatch);
}

BlendMode GetBlendMode(BlendingConfig blendConfig, bool preMultipliedAlpha) {
    BlendMode mode = {BlendType::NONE, BlendType::NONE};
    switch(blendConfig) {
        case BlendingConfig::Normal: {
            mode.src = preMultipliedAlpha ? BlendType::ONE : BlendType::SRC_ALPHA;
            mode.dst = BlendType::ONE_MINUS_SRC_ALPHA;
            break;
        }
        case BlendingConfig::Additive: {
            mode.src = preMultipliedAlpha ? BlendType::ONE : BlendType::SRC_ALPHA;
            mode.dst = BlendType::ONE;
            break;
        }
        case BlendingConfig::Screen: {
            mode.src = BlendType::ONE;
            mode.dst = BlendType::ONE_MINUS_SRC_COLOR;
            break;
        }
        case BlendingConfig::Multiply:
            mode.src = BlendType::DST_COLOR;
            mode.dst = BlendType::ONE_MINUS_SRC_ALPHA;
            break;
        default: {
            assert(false && "Invalid blend mode");
        }
    }
    return mode;
}

} // namespace RenderUtils