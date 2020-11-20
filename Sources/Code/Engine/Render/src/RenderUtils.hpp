#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Render/Color.hpp"

class ImageBuffer;
class RenderFramebuffer;
class Node;
class RenderContext;

namespace RenderUtils {

const char* GetGLError();

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void DrawWithMask(Node& node, RenderContext& ctx);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */