#ifndef __IMAGE_NODE_HPP__
#define __IMAGE_NODE_HPP__

#include "Nodes/Node.hpp"
#include "Render/TextureInfo.hpp"

class RenderTexture;

class ImageNode : public Node {
public:

    ImageNode();
    virtual ~ImageNode();

    void setSize(const Vec2& newSize);
    void setTextureInfo(const TextureInfo& newTextureInfo);

    // Node
    bool canRender() const override;

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

protected:

    TextureInfo texInfo;
    std::shared_ptr<RenderTexture> texObj;
    Vec2 size;
};

#endif /* __IMAGE_NODE_HPP__ */