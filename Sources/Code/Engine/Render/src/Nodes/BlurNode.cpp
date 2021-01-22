#include "Nodes/BlurNode.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderTexture.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderManager.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderUtils.hpp"

#include <cassert>

namespace {

void resizeFBOTexture(RenderFramebuffer& fbo, const Vec2i& newSize) {
    if(fbo.color0.getSize() == newSize) {
        return;
    }
    fbo.color0.bind();
    if(!fbo.color0.resize(newSize)) {
        LogError("[resizeFBOTexture] Can't resize fbo to a size: %dx%d", newSize.x, newSize.y);
    }
    fbo.color0.unbind();
}

} // namespace

BlurNode::BlurNode() :
    passes(1),
    downScale(2) {
}

BlurNode::~BlurNode() {
}

void BlurNode::onInit() {
    setShader("blur");
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
}

void BlurNode::setDownScale(int newDownScale) {
    assert(newDownScale >= 1 && "Invalid down scale value");
    downScale = newDownScale;
}

void BlurNode::setPasses(int newPassesCount) {
    assert(newPassesCount >= 1 && "Invalid passes count value");
    passes = newPassesCount;
}

bool BlurNode::isVisible() const {
    if(passes == 0) {
        return false;
    }
    return Node::isVisible();
}

void BlurNode::blurPass(RenderFramebuffer& first, RenderFramebuffer& second) {
    Vec2i size = first.color0.getSize();
    {
        first.bind();
        shader->setTexture2D(UniformType::Texture, second.color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 1);
        shader->setUniform2f(UniformType::TextureSize, size);
        geom->draw();
        first.unbind();
    }
    {
        second.bind();
        shader->setTexture2D(UniformType::Texture, first.color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 0);
        geom->draw();
        second.unbind();
    }
}

void BlurNode::onRender(RenderContext& ctx) {
    auto mainFBO = ctx.mainFBO;
    Vec2i size = mainFBO->color0.getSize();
    Vec2i scaledSize = size / downScale;

    auto extraFBO = ctx.exraFBOs[0];
    resizeFBOTexture(*extraFBO, scaledSize);

    RenderUtils::BlitFromFBOtoFBO(*mainFBO, *extraFBO);

    glViewport(0, 0, scaledSize.x, scaledSize.y);
    for(int i = 0; i < passes; ++i) {
        // blurPass(*extraFBO, *mainFBO);
    }
    glViewport(0, 0, size.x, size.y);

    RenderUtils::BlitFromFBOtoFBO(*extraFBO, *ctx.mainFBO);

    ctx.mainFBO->bind();
}