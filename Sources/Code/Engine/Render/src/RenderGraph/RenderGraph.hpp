#ifndef __RENDER_GRAPH_HPP__
#define __RENDER_GRAPH_HPP__

#include "RenderContext.hpp"

#include <vector>

class RenderNode;

class RenderGraph {
public:

    RenderGraph();
    ~RenderGraph();

    void addChild(RenderNode* node);
    void removeChild(RenderNode* node);

    void render();

    RenderContext& getContext();
    const RenderContext& getContext() const;
    void reorderNodes();

private:

    RenderContext ctx;
    std::vector<RenderNode*> children;
    bool needReorder;
};

#endif /* __RENDER_GRAPH_HPP__ */