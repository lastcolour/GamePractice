#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

#include "Render/Color.hpp"

class ImageBuffer;
class RenderFramebuffer;

namespace RenderUtils {

const char* GetGLError();

bool IsOpenGLContextExists();

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

void BlitFromFBOtoFBO(RenderFramebuffer& fromFBO, RenderFramebuffer& toFBO);

void BlitFromFBOtoDefaultFBO(RenderFramebuffer& fromFBO);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */