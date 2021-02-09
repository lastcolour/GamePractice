#ifndef __NINE_PATCH_NODE_HPP__
#define __NINE_PATCH_NODE_HPP__

#include "Nodes/ImageNode.hpp"

class NinePatchNode : public ImageNode {
public:

    NinePatchNode();
    virtual ~NinePatchNode();

    void setPatches(float newHorizontal, float newVertical, float newPatchScale);

protected:

    // ImageNode
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

protected:

    Vec2 tmScale;
    float horizontal;
    float vertical;
    float patchScale;
};

#endif /* __NINE_PATCH_NODE_HPP__ */