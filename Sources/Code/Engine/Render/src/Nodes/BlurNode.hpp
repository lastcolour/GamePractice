#ifndef __BLUR_NODE_HPP__
#define __BLUR_NODE_HPP__

#include "Nodes/Node.hpp"
#include "Nodes/CopyFBOtoFBO.hpp"

class BlurNode : public Node {
public:

    BlurNode();
    virtual ~BlurNode();

    void setDownScale(int newDownScale);
    void setPasses(int newPassesCount);

    // Node
    bool canRender() const override;

protected:

    // Node
    void onRenderStart(RenderContext& ctx) override {}
    void onRenderEnd(RenderContext& ctx) override {}
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

private:

    void blurPass(RenderFramebuffer& first, RenderFramebuffer& second);

private:

    CopyFBOtoFBO fboCopyLogic;
    int passes;
    int downScale;
};

#endif /* __BLUR_NODE_HPP__ */