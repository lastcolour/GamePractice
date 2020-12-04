#ifndef __NINE_PATCH_NODE_HPP__
#define __NINE_PATCH_NODE_HPP__

#include "Nodes/ImageNode.hpp"

class NinePatchNode : public ImageNode {
public:

    NinePatchNode();
    virtual ~NinePatchNode();

    void setPatches(float newLeft, float newRight, float newTop, float newDown);

protected:

    // ImageNode
    void onInit() override;
    void onRender(RenderContext& ctx) override;

private:

    float left;
    float right;
    float top;
    float down;
};

#endif /* __NINE_PATCH_NODE_HPP__ */