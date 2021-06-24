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
    void setText(std::string&& newText);
    void setText(const std::string& newText);
    void setAlignAtCenter(bool flag);
    void setFontType(EFontType newFontType);

    // Node
    bool canRender() const override;

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

private:

    void drawLines();
    unsigned int drawLine(const Vec2& pt, unsigned int vertShift, size_t startIdx, size_t endIdx);

private:

    std::shared_ptr<RenderFont> font;
    std::vector<Vec4> vertData;
    std::string text;
    TextMetric textMetric;
    ColorB color;
    float fontHeight;
    bool alignAtCenter;
};

#endif /* __TEXT_NODE_HPP__ */