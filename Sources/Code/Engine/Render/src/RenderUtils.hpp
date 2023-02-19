#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Math/Matrix.hpp"
#include "Render/RenderCommon.hpp"
#include "Platform/OpenGL.hpp"
#include "VertTypes.hpp"

class ImageBuffer;
class RenderFramebuffer;
class RenderGeometry;
class RenderTexture;

namespace RenderUtils {

const float TextNewLineOffset = 1.f;

const int MaxLinesPerDraw = 8192;

const int MaxCharsPerDraw = 512;

const int NinePatchesVertCount = 2 * 3 * 9;

const char* GetGLError();

const char* GetGLFBOError(GLenum framebufferType);

bool IsOpenGLContextExists();

GLenum GetGLBlendMode(EBlendOp blendOp);

GLenum GetGLTexWrapType(ETextureWrapType wrapType);

GLenum GetGLTexLerpType(ETextureLerpType lerpType);

BlendOpPair GetBlendOpPair(EBlendMode blendMode, bool preMultipliedAlpha);

bool ReadFramebufferToBuffer(RenderFramebuffer& framebuffer, void* out);

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO);

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO);

std::shared_ptr<RenderTexture> CreateTexture(const TextureInfo& tex, ETextureDataType texType);

Vec2 GetNinePatchVertexCoord(const Vec2i& imageSize, const Vec2& drawSize, const Vec2& patches);

int CreateNinePatchVertData(const Vec2& patchPt, const Vec2& patchUV, Vert_Vec3_Tex* out);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */