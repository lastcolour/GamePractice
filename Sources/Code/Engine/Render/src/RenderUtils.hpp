#ifndef __RENDER_UTILS_HPP__
#define __RENDER_UTILS_HPP__

class ImageBuffer;
class RenderFramebuffer;

namespace RenderUtils {

const char* GetGLError();

bool ReadFramebufferToImage(RenderFramebuffer& framebuffer, ImageBuffer& imageBuffer);

} // namespace RenderUtils

#endif /*__RENDER_UTILS_HPP__ */