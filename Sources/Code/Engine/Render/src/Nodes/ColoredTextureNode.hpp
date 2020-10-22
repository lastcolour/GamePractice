#ifndef __COLORED_TEXTURE_HPP__
#define __COLORED_TEXTURE_HPP__

#include "Nodes/Node.hpp"

#include <memory>

class RenderTexture;

class ColoredTextureNode : public Node {
public:

    ColoredTextureNode();
    virtual ~ColoredTextureNode();

    void ET_setColor0(const ColorB& newColor) override;
    void ET_setSize(const Vec2i& newSize) override;
    void ET_setImage(const std::string& newImage) override;

protected:

    void onInitRender() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2i size;
    ColorB color;
};

#endif /* __COLORED_TEXTURE_HPP__ */