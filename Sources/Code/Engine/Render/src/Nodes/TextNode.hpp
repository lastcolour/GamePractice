#ifndef __TEXT_NODE_HPP__
#define __TEXT_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderFont.hpp"

class TextNode : public Node {
public:

    TextNode();
    virtual ~TextNode();

    void ET_setFontHeight(int newHeight) override;
    void ET_setColor0(const ColorB& newColor) override;
    void ET_setText(const std::string& newText) override;

    void setAlignAtCenter(bool flag);

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

    void drawLines();
    unsigned int drawLine(const Vec2& pt, const Vec2& scale, unsigned int vertShift, Vec4* vertChunk, size_t startIdx, size_t endIdx);

private:

    std::shared_ptr<RenderFont> font;
    std::string text;
    ColorB color;
    int fontHeight;
    bool alignAtCenter;
};

#endif /* __TEXT_NODE_HPP__ */