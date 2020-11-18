#ifndef __DRAW_FRAME_NODE_HPP__
#define __DRAW_FRAME_NODE_HPP__

#include <memory>

class RenderShader;
class RenderGeometry;
class RenderFramebuffer;

class DrawFrameNode {
public:

    DrawFrameNode();
    ~DrawFrameNode();

    bool init();
    void draw(RenderFramebuffer& framebuffer);

private:

    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /*  __DRAW_FRAME_NODE_HPP__ */
