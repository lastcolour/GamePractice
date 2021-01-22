#ifndef __ET_RENDER_INTERFACES_HPP__
#define __ET_RENDER_INTERFACES_HPP__

class ImageBuffer;

enum class DrawContentFilter {
    None = 0,
    NoDebugInfo
};

struct ETRender {
    virtual ~ETRender() = default;
    virtual void ET_drawFrameToBuffer(ImageBuffer& imageBuffer, const Vec2i& drawSize, DrawContentFilter filter) = 0;
};

struct ETRenderContextEvents {
    virtual ~ETRenderContextEvents() = default;
    virtual void ET_onContextCreated() = 0;
    virtual void ET_onContextDestroyed() = 0;
};

#endif /* __ET_RENDER_INTERFACES_HPP__ */