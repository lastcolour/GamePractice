#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Math/Matrix.hpp"
#include "Render/TextureInfo.hpp"

class ImageBuffer;
class RenderFramebuffer;
class RenderGeometry;
class RenderTexture;
class Node;

enum class BlendingConfig;
struct BlendMode;

enum class RenderNodeType {
    Quad = 0,
    Simple,
    Image,
    ColoredTexture,
    Gradient,
    Text,
    ParticleEmmiter,
    Blur,
    NinePatchImage,
    LightingBolt
};

namespace RenderUtils {

const float TextNewLineOffset = 1.f;

const int MaxLinesPerDraw = 8192;

const int MaxCharsPerDraw = 512;

const char* GetGLError();

bool IsOpenGLContextExists();

bool ReadFramebufferToBuffer(RenderFramebuffer& framebuffer, void* out);

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO);

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO);

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale);

std::unique_ptr<Node> CreateRenderNode(RenderNodeType nodeType);

std::shared_ptr<RenderTexture> CreateTexture(const TextureInfo& tex, ETextureDataType texType);

void ApplyTextureInfo(RenderTexture& texObj, const TextureInfo& texInfo);

Vec2 GetNinePatchVertexCoord(const Vec2i& imageSize, const Vec2& drawSize, const Vec2& patches, float patchScale);

BlendMode GetBlendMode(BlendingConfig blendConfig, bool preMultipliedAlpha);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */