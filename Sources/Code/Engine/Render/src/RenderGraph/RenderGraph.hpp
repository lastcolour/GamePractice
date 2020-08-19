#ifndef __RENDER_GRAPH_HPP__
#define __RENDER_GRAPH_HPP__

#include "RenderContext.hpp"

#include <vector>

class Node;

class RenderGraph {
public:

    RenderGraph();
    ~RenderGraph();

    void addChild(Node* node);
    void removeChild(Node* node);

    void render();

    RenderContext& getContext();
    const RenderContext& getContext() const;
    void setNeedReorderNodes();

private:

    RenderContext ctx;
    std::vector<Node*> children;
    bool needReorder;
};

#endif /* __RENDER_GRAPH_HPP__ */