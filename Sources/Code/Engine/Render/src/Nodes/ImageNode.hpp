#ifndef __IMAGE_NODE_HPP__
#define __IMAGE_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"

class ImageNode : public Node {
public:

    ImageNode();
    virtual ~ImageNode();

    void setSize(const Vec2& newSize);
    void setImage(const std::string& newImage);
    void setTintColor(const ColorB& newTintColor);
    void setTexture(std::shared_ptr<RenderTexture>& newTex);
    std::shared_ptr<RenderTexture> getTexture();

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

protected:

    std::shared_ptr<RenderTexture> tex;
    Vec2 size;
    ColorB tintColor;
};

#endif /* __IMAGE_NODE_HPP__ */