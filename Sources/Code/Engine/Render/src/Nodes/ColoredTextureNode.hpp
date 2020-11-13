#ifndef __COLORED_TEXTURE_HPP__
#define __COLORED_TEXTURE_HPP__

#include "Nodes/Node.hpp"

#include <memory>

class RenderTexture;

class ColoredTextureNode : public Node {
public:

    ColoredTextureNode();
    virtual ~ColoredTextureNode();

    void setColor0(const ColorB& newColor);
    void setSize(const Vec2& newSize);
    void setImage(const std::string& newImage);

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2 size;
    ColorB color;
};

#endif /* __COLORED_TEXTURE_HPP__ */