#ifndef __IMAGE_NODE_HPP__
#define __IMAGE_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"

class ImageNode : public Node {
public:

    ImageNode();
    virtual ~ImageNode();

    // ETRenderNodeProxy
    void ET_setSize(const Vec2i& newSize) override;
    void ET_setImage(const std::string& newImage) override;

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2i size;
};

#endif /* __IMAGE_NODE_HPP__ */