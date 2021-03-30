#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Math/Matrix.hpp"
#include "Render/Color.hpp"

class ImageBuffer;
class RenderFramebuffer;
class RenderGeometry;
class Node;

enum class RenderNodeType {
    Simple = 0,
    Image,
    ColoredTexture,
    Gradient,
    Text,
    ParticleEmmiter,
    Blur,
    NinePatchImage
};

namespace RenderUtils {

const float TextNewLineOffset = 1.f;

const int MaxParticlessPerDraw = 1024;

const char* GetGLError();

bool IsOpenGLContextExists();

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO);

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO);

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale);

std::unique_ptr<Node> CreateRenderNode(RenderNodeType nodeType);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */