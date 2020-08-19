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

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    std::shared_ptr<RenderFont> font;
    std::string text;
    ColorB color;
    int fontHeight;
};

#endif /* __TEXT_NODE_HPP__ */