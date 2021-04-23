#ifndef __LINE_NODE_HPP__
#define __LINE_NODE_HPP__

#include "Nodes/Node.hpp"

struct RenderLine {
    Vec2 startPt;
    ColorF startCol;
    Vec2 endPt;
    ColorF endCol;
};

class LineNode : public Node {
public:

    LineNode();
    virtual ~LineNode();

    void renderLines(const std::vector<RenderLine>& lines, RenderContext& ctx);

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override {}
    Mat4 calcModelMat(const Transform& newTm) override;
};

#endif /* __LINE_NODE_HPP__ */