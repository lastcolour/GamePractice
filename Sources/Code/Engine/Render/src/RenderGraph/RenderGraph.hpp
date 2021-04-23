#ifndef __RENDER_GRAPH_HPP__
#define __RENDER_GRAPH_HPP__

#include "RenderGraph/RenderContext.hpp"
#include "Render/ETRenderInterfaces.hpp"

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
    void renderToBuffer(void* outBuffer, DrawContentFilter filter);

    void setNeedReorderNodes();

private:

    void drawToFBO(DrawContentFilter filter);
    void prepareNodes();
    bool startFrame();
    void endFrame();

private:

    RenderContext ctx;
    std::vector<Node*> children;
    std::shared_ptr<RenderFramebuffer> mainFBO;
    std::vector<std::shared_ptr<RenderFramebuffer>> extraFBOs;
    bool needReorder;
    ColorF clearColor;
    DrawContentFilter drawFilter;
};

#endif /* __RENDER_GRAPH_HPP__ */