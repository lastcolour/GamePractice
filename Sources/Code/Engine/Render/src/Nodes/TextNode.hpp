#ifndef __TEXT_NODE_HPP__
#define __TEXT_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderFont.hpp"

class TextNode : public Node {
public:

    TextNode();
    virtual ~TextNode();

    void setFontHeight(float newFontHeight);
    void setColor0(const ColorB& newColor);
    void setText(const std::string& newText);
    void setAlignAtCenter(bool flag);

    // Node
    bool canRender() const override;

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
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
    float fontHeight;
    bool alignAtCenter;
    bool doUpdate;
};

#endif /* __TEXT_NODE_HPP__ */