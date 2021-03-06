#ifndef __GRADIENT_NODE_HPP__
#define __GRADIENT_NODE_HPP__

#include "Nodes/Node.hpp"

#include <memory>

class RenderTexture;

class GradientNode : public Node {
public:

    GradientNode();
    virtual ~GradientNode();

    void setColor0(const ColorB& newColor);
    void setColor1(const ColorB& newColor);
    void setSize(const Vec2& newSize);
    void setVertical(bool flag);

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    void updateTexData();

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2 size;
    ColorB startCol;
    ColorB endCol;
    bool isVertical;
    bool doUpdate;
};

#endif /* __GRADIENT_NODE_HPP__ */