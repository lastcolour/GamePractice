#ifndef __GRADIENT_NODE_HPP__
#define __GRADIENT_NODE_HPP__

#include "Nodes/Node.hpp"

class RenderTexture;

class GradientNode : public Node {
public:

    GradientNode();
    virtual ~GradientNode();

    void setColor0(const ColorB& newColor);
    void setColor1(const ColorB& newColor);
    void setSize(const Vec2& newSize);
    void setVertical(bool flag);

    // Node
    bool canRender() const override;

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

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