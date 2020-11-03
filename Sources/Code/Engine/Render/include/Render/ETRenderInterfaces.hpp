#ifndef __ET_RENDER_INTERFACES_HPP__
#define __ET_RENDER_INTERFACES_HPP__

class RenderTextureFramebuffer;

enum class DrawContentFilter {
    None = 0,
    NoDebugInfo
};

struct ETRender {
    virtual ~ETRender() = default;
    virtual void ET_drawFrameToFramebuffer(RenderTextureFramebuffer& renderFb, DrawContentFilter filter) = 0;
};

struct ETRenderContextEvents {
    virtual ~ETRenderContextEvents() = default;
    virtual void ET_onContextSuspended() = 0;
    virtual void ET_onContextRestored() = 0;
    virtual void ET_onContextReCreated() = 0;
};

#endif /* __ET_RENDER_INTERFACES_HPP__ */