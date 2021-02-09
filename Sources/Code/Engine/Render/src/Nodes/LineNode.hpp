#ifndef __LINE_NODE_HPP__
#define __LINE_NODE_HPP__

#include "Nodes/Node.hpp"

#include <vector>

class LineNode : public Node {
public:

    LineNode();
    virtual ~LineNode();

    void setLine(const Vec2& newStartPt, const Vec2& newEndPt, const ColorB& newCol, float newWidth);

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

private:

    Vec2 startPt;
    Vec2 endPt;
    ColorB col;
    float width;
};

#endif /* __LINE_NODE_HPP__ */