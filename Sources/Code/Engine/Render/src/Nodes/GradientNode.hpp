#ifndef __GRADIENT_NODE_HPP__
#define __GRADIENT_NODE_HPP__

#include "Nodes/Node.hpp"

#include <memory>

class RenderTexture;

class GradientNode : public Node {
public:

    GradientNode();
    virtual ~GradientNode();

    void ET_setColor0(const ColorB& newColor) override;
    void ET_setColor1(const ColorB& newColor) override;
    void ET_setSize(const Vec2i& newSize) override;
    void ET_setVertical(bool flag) override;

protected:

    void onInitRender() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    void updateTexData();

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2i size;
    ColorB startCol;
    ColorB endCol;
    bool isVertical;
    bool doUpdate;
};

#endif /* __GRADIENT_NODE_HPP__ */