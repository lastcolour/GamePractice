#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Render/Color.hpp"

class ImageBuffer;
class RenderFramebuffer;

namespace RenderUtils {

const char* GetGLError();

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void ClearFramebuffer(const ColorF& clearColor, RenderFramebuffer& framebuffer);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */