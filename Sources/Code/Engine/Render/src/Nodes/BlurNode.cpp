#include "Nodes/BlurNode.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderTexture.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderManager.hpp"
#include "Platform/OpenGL.hpp"

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
    ET_SendEventReturn(drawShader, &ETRenderShaderManager::ET_createShader, "draw_framebuffer");
    if(!drawShader) {
        return;
    }
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

void BlurNode::downSamplePass(RenderFramebuffer& mainFB0, RenderFramebuffer& targetFBO) {
    drawShader->bind();
    targetFBO.bind();
    drawShader->setTexture2D(UniformType::Texture, mainFB0.color0);
    geom->draw();
    drawShader->unbind();
}

bool BlurNode::isVisible() const {
    if(passes == 0) {
        return false;
    }
    if(!drawShader) {
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

void BlurNode::upSamplePass(RenderFramebuffer& mainFB0, RenderFramebuffer& sourceFBO) {
    drawShader->bind();
    mainFB0.bind();
    drawShader->setTexture2D(UniformType::Texture, sourceFBO.color0);
    geom->draw();
    drawShader->unbind();
}

void BlurNode::onRender(RenderContext& ctx) {
    Vec2i size = ctx.mainFBO->color0.getSize();
    Vec2i scaledSize = size / downScale;

    auto fbo0 = ctx.exraFBOs[0];
    resizeFBOTexture(*fbo0, scaledSize);

    auto fbo1 = ctx.exraFBOs[1];
    resizeFBOTexture(*fbo1, scaledSize);

    glViewport(0, 0, scaledSize.x, scaledSize.y);
    downSamplePass(*ctx.mainFBO, *fbo0);

    shader->bind();
    for(int i = 0; i < passes; ++i) {
        blurPass(*fbo1, *fbo0);
    }
    shader->unbind();

    Vec2i origViewPort = ctx.mainFBO->color0.getSize();
    glViewport(0, 0, origViewPort.x, origViewPort.y);
    upSamplePass(*ctx.mainFBO, *fbo0);
}