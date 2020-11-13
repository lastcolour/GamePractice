#ifndef __SIMPLE_NODE_HPP__
#define __SIMPLE_NODE_HPP__

#include "Nodes/Node.hpp"

class SimpleNode : public Node {
public:

    SimpleNode();
    virtual ~SimpleNode();

    void setColor0(const ColorB& newColor);
    void setSize(const Vec2& newSize);

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;

private:

    Vec2 size;
    ColorB color;
};

#endif /* __SIMPLE_NODE_HPP__ */