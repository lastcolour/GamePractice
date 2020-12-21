#ifndef __BLUR_NODE_HPP__
#define __BLUR_NODE_HPP__

#include "Nodes/Node.hpp"

class BlurNode : public Node {
public:

    BlurNode();
    virtual ~BlurNode();

    void setDownScale(int newDownScale);
    void setPasses(int newPassesCount);

protected:

    // Node
    void onInit() override;
    void onRenderStart(RenderContext& ctx) override {}
    void onRender(RenderContext& ctx) override;
    void onRenderEnd(RenderContext& ctx) override {}
    bool isVisible() const override;

private:

    void downSamplePass(RenderFramebuffer& mainFB0, RenderFramebuffer& targetFBO);
    void blurPass(RenderFramebuffer& first, RenderFramebuffer& second);
    void upSamplePass(RenderFramebuffer& mainFB0, RenderFramebuffer& sourceFBO);

private:

    std::shared_ptr<RenderShader> drawShader;
    int passes;
    int downScale;
};

#endif /* __BLUR_NODE_HPP__ */