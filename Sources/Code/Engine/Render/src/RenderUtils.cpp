#include "RenderUtils.hpp"
#include "RenderFramebuffer.hpp"
#include "Render/ImageBuffer.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Platform/ETSurface.hpp"
#include "RenderGeometry.hpp"
#include "Math/MatrixTransform.hpp"
#include "RenderTexture.hpp"
#include "Render/ParticlesEmitterConfig.hpp"

#include <cassert>

static_assert(std::is_same<int, GLsizei>::value, "int != GLsizei");
static_assert(std::is_same<int, GLint>::value, "int != GLint");
static_assert(std::is_same<float, GLfloat>::value, "float != GLfloat");
static_assert(std::is_same<unsigned int, GLuint>::value, "unsigned int != GLuint");

namespace RenderUtils {

const char* GetGLFBOError(GLenum framebufferType) {
    GLenum errCode = glCheckFramebufferStatus(framebufferType);
    const char* resErrStr = nullptr;
    switch(errCode) {
        case GL_FRAMEBUFFER_COMPLETE: {
            break;
        }
        case GL_FRAMEBUFFER_UNDEFINED: {
            resErrStr = "GL_FRAMEBUFFER_UNDEFINED";
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
            resErrStr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
            resErrStr = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED: {
            resErrStr = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
            resErrStr = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        }
        default: {
            resErrStr = "Uknown";
            break;
        }
    }
    return resErrStr;
}

const char* GetGLError() {
    GLenum errCode = glGetError();
    const char* resErrStr = nullptr;
    switch(errCode) {
        case GL_NO_ERROR: {
            break;
        }
        case GL_INVALID_ENUM: {
            resErrStr = "GL_INVALID_ENUM";
            break;
        }
        case GL_INVALID_VALUE: {
            resErrStr = "GL_INVALID_VALUE";
            break;
        }
        case GL_INVALID_OPERATION: {
            resErrStr = "GL_INVALID_OPERATION";
            break;
        }
        case GL_OUT_OF_MEMORY: {
            resErrStr = "GL_OUT_OF_MEMORY";
            break;
        }
        case GL_INVALID_FRAMEBUFFER_OPERATION: {
            resErrStr = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        default: {
            resErrStr = "Unknown";
            break;
        }
    }
    return resErrStr;
}

bool IsOpenGLContextExists() {
    bool hasContext = false;
    ET_SendEventReturn(hasContext, &ETSurface::ET_canRender);
    return hasContext;
}

GLenum GetGLTexLerpType(ETextureLerpType lerpType) {
    GLenum resLerpType = GL_NONE;
    switch(lerpType)
    {
        case ETextureLerpType::Linear: {
            resLerpType = GL_LINEAR;
            break;
        }
        case ETextureLerpType::Point: {
            resLerpType = GL_NEAREST;
            break;
        }
        default: {
            assert(false && "Invalid texture lerp type");
        }
    }
    return resLerpType;
}

GLenum GetGLTexWrapType(ETextureWrapType wrapType) {
    GLenum resWrapType = GL_NONE;
    switch(wrapType) {
        case ETextureWrapType::Repeat: {
            resWrapType = GL_REPEAT;
            break;
        }
        case ETextureWrapType::MirroredRepeat: {
            resWrapType = GL_MIRRORED_REPEAT;
            break;
        }
        case ETextureWrapType::ClampToEdge: {
            resWrapType = GL_CLAMP_TO_EDGE;
            break;
        }
        default: {
            assert(false && "Invalid texture wrap type");
        }
    }
    return resWrapType;
}

GLenum GetGLBlendMode(EBlendOp blendOp) {
    GLenum resBlenOp = GL_ONE;
    switch(blendOp) {
        case EBlendOp::NONE: {
            assert(false && "Invalid blend mode");
        }
        case EBlendOp::ZERO: {
            resBlenOp = GL_ZERO;
            break;
        }
        case EBlendOp::ONE: {
            resBlenOp = GL_ONE;
            break;
        }
        case EBlendOp::SRC_COLOR: {
            resBlenOp = GL_SRC_COLOR;
            break;
        }
        case EBlendOp::ONE_MINUS_SRC_COLOR: {
            resBlenOp = GL_ONE_MINUS_SRC_COLOR;
            break;
        }
        case EBlendOp::DST_COLOR: {
            resBlenOp = GL_DST_COLOR;
            break;
        }
        case EBlendOp::ONE_MINUS_DST_COLOR: {
            resBlenOp = GL_ONE_MINUS_DST_COLOR;
            break;
        }
        case EBlendOp::SRC_ALPHA: {
            resBlenOp = GL_SRC_ALPHA;
            break;
        }
        case EBlendOp::ONE_MINUS_SRC_ALPHA: {
            resBlenOp = GL_ONE_MINUS_SRC_ALPHA;
            break;
        }
        case EBlendOp::DST_ALPHA: {
            resBlenOp = GL_DST_ALPHA;
            break;
        }
        case EBlendOp::ONE_MINUS_DST_ALPHA: {
            resBlenOp = GL_ONE_MINUS_DST_ALPHA;
            break;
        }
        case EBlendOp::CONSTANT_COLOR: {
            resBlenOp = GL_CONSTANT_COLOR;
            break;
        }
        case EBlendOp::ONE_MINUS_CONSTANT_COLOR: {
            resBlenOp = GL_ONE_MINUS_CONSTANT_COLOR;
            break;
        }
        case EBlendOp::CONSTANT_ALPHA: {
            resBlenOp = GL_CONSTANT_ALPHA;
            break;
        }
        case EBlendOp::ONE_MINUS_CONSTANT_ALPHA: {
            resBlenOp = GL_ONE_MINUS_CONSTANT_ALPHA;
            break;
        }
        default: {
            assert(false && "Invalid blend mode");
        }
    }
    return resBlenOp;
}

BlendOpPair GetBlendOpPair(EBlendMode blendMode, bool preMultipliedAlpha) {
    BlendOpPair resOpPair = {EBlendOp::NONE, EBlendOp::NONE};
    switch(blendMode) {
        case EBlendMode::None: {
            break;
        }
        case EBlendMode::Normal: {
            resOpPair.src = preMultipliedAlpha ? EBlendOp::ONE : EBlendOp::SRC_ALPHA;
            resOpPair.dst = EBlendOp::ONE_MINUS_SRC_ALPHA;
            break;
        }
        case EBlendMode::Additive: {
            resOpPair.src = preMultipliedAlpha ? EBlendOp::ONE : EBlendOp::SRC_ALPHA;
            resOpPair.dst = EBlendOp::ONE;
            break;
        }
        case EBlendMode::Screen: {
            resOpPair.src = EBlendOp::ONE;
            resOpPair.dst = EBlendOp::ONE_MINUS_SRC_COLOR;
            break;
        }
        case EBlendMode::Multiply:
            resOpPair.src = EBlendOp::DST_COLOR;
            resOpPair.dst = EBlendOp::ONE_MINUS_SRC_ALPHA;
            break;
        default: {
            assert(false && "Invalid blend mode");
        }
    }
    return resOpPair;
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
    if(auto err = GetGLFBOError(GL_READ_FRAMEBUFFER)) {
        LogError("[BlitFromFBOtoFBO] Invalid from FBO (Error: %s)", err);
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, toFBO.framebufferId);
    if(auto err = GetGLFBOError(GL_DRAW_FRAMEBUFFER)) {
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
    if(auto err = GetGLFBOError(GL_READ_FRAMEBUFFER)) {
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
    float r = (imageSize.x * drawSize.y) / (imageSize.y * drawSize.x);

    float h1 = 0.f;
    if(imageSize.x <= drawSize.x) {
        h1 = imageSize.x * patches.x / drawSize.x;
        h1 = Math::Lerp(h1, patches.y * r, patchScale);
    } else {
        h1 = 0.5f * r;
    }

    float v1 = 0.f;
    if(imageSize.y <= drawSize.y) {
        v1 = imageSize.y * patches.y / drawSize.y;
        v1 = Math::Lerp(v1, patches.y, patchScale);
    } else {
        v1 = 0.5f;
    }

    float s = 1.f;

    if(h1 >= 0.5f) {
        s = 0.5f / h1;
    }
    if(v1 >= 0.5f) {
        s = std::min(s, 0.5f / v1);
    }

    h1 *= s;
    v1 *= s;

    return Vec2(h1, v1);
}

const char* GetNameOfDrawCmdType(EDrawCmdType cmdType) {
    const char* resName = "Unknown";
    switch(cmdType) {
        case EDrawCmdType::Text: {
            resName = "Text";
            break;
        }
        case EDrawCmdType::TexturedQuad: {
            resName = "TexturedQuad";
            break;
        }
        case EDrawCmdType::Quad: {
            resName = "Quad";
            break;
        }
        case EDrawCmdType::Particles: {
            resName = "Particles";
            break;
        }
        case EDrawCmdType::Blur: {
            resName = "Blur";
            break;
        }
        default: {
            assert(false && "Invalid name");
        }
    }
    return resName;
}

} // namespace RenderUtils