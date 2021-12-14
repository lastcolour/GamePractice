#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Math/Matrix.hpp"
#include "Render/RenderCommon.hpp"
#include "Commands/Commands.hpp"
#include "Platform/OpenGL.hpp"

class ImageBuffer;
class RenderFramebuffer;
class RenderGeometry;
class RenderTexture;

namespace RenderUtils {

const float TextNewLineOffset = 1.f;

const int MaxLinesPerDraw = 8192;

const int MaxCharsPerDraw = 512;

const char* GetGLError();

const char* GetGLFBOError(GLenum framebufferType);

bool IsOpenGLContextExists();

GLenum GetGLBlendOp(EBlendOp blendOp);

GLenum GetGLTexWrapType(ETextureWrapType wrapType);

GLenum GetGLTexLerpType(ETextureLerpType lerpType);

const char* GetDrawCmdTypeName(EDrawCmdType cmdType);

BlendOpPair GetBlendOpPair(EBlendMode blendMode, bool preMultipliedAlpha);

bool ReadFramebufferToBuffer(RenderFramebuffer& framebuffer, void* out);

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO);

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO);

std::shared_ptr<RenderTexture> CreateTexture(const TextureInfo& tex, ETextureDataType texType);

void ApplyTextureInfo(RenderTexture& texObj, const TextureInfo& texInfo);

Vec2 GetNinePatchVertexCoord(const Vec2i& imageSize, const Vec2& drawSize, const Vec2& patches, float patchScale);

const char* GetNameOfDrawCmdType(EDrawCmdType cmdType);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */