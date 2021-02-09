#ifndef __TEXT_NODE_HPP__
#define __TEXT_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderFont.hpp"

class TextNode : public Node {
public:

    TextNode();
    virtual ~TextNode();

    void setFontHeight(int newHeight);
    void setColor0(const ColorB& newColor);
    void setText(const std::string& newText);
    void setAlignAtCenter(bool flag);

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;
    Mat4 calcModelMat(const Transform& newTm) override;

private:

    void drawLines();
    unsigned int drawLine(const Vec2& pt, const Vec2& scale, unsigned int vertShift, Vec4* vertChunk, size_t startIdx, size_t endIdx);

private:

    std::shared_ptr<RenderFont> font;
    std::string text;
    TextMetric textMetric;
    ColorB color;
    Vec2 tmPt;
    Vec2 tmScale;
    int fontHeight;
    bool alignAtCenter;
    bool doUpdate;
};

#endif /* __TEXT_NODE_HPP__ */