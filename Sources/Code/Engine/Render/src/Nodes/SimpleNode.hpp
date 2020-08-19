#ifndef __SIMPLE_NODE_HPP__
#define __SIMPLE_NODE_HPP__

#include "Nodes/Node.hpp"

class SimpleNode : public Node {
public:

    SimpleNode();
    virtual ~SimpleNode();

    // ETRenderProxyNode
    void ET_setColor0(const ColorB& newColor) override;
    void ET_setSize(const Vec2i& newSize) override;

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override;

private:

    Vec2i size;
    ColorB color;
};

#endif /* __SIMPLE_NODE_HPP__ */