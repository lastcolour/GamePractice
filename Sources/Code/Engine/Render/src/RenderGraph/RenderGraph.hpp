#ifndef __RENDER_GRAPH_HPP__
#define __RENDER_GRAPH_HPP__

#include "RenderGraph/RenderContext.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "RenderGraph/DrawFrameNode.hpp"

#include <vector>
#include <memory>

class Node;
class RenderFramebuffer;

class RenderGraph {
public:

    RenderGraph();
    ~RenderGraph();

    void init();
    void addChild(Node* node);
    void removeChild(Node* node);

    void render();
    void renderToBuffer(ImageBuffer& imageBuffer, DrawContentFilter filter);

    void setNeedReorderNodes();

private:

    void prepareNodes();
    bool startFrame();
    void endFrame();

private:

    RenderContext ctx;
    DrawFrameNode drawFrameNode;
    std::vector<Node*> children;
    std::shared_ptr<RenderFramebuffer> mainFBO;
    std::vector<std::shared_ptr<RenderFramebuffer>> extraFBOs;
    bool needReorder;
    ColorF clearColor;
};

#endif /* __RENDER_GRAPH_HPP__ */