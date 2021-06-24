#include "Nodes/BlurNode.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderTexture.hpp"
#include "Render/ETRenderManager.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderUtils.hpp"

#include <cassert>

BlurNode::BlurNode() :
    passes(1),
    downScale(2) {
}

BlurNode::~BlurNode() {
}

void BlurNode::onInit() {
    setShader("blur");
    setGeometry(PrimitiveGeometryType::Sqaure_Tex);
    fboCopyLogic.init();
}

void BlurNode::setDownScale(int newDownScale) {
    assert(newDownScale >= 1 && "Invalid down scale value");
    downScale = newDownScale;
}

void BlurNode::setPasses(int newPassesCount) {
    assert(newPassesCount >= 1 && "Invalid passes count value");
    passes = newPassesCount;
}

Mat4 BlurNode::calcModelMat(const Transform& newTm) {
    return Mat4(1.f);
}

bool BlurNode::canRender() const {
    if(passes == 0) {
        return false;
    }
    return Node::canRender();
}

void BlurNode::blurPass(RenderFramebuffer& first, RenderFramebuffer& second) {
    Vec2i size = second.color0.getSize();

    {
        first.bind();
        shader->setTexture2d(UniformType::Texture, 0, second.color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 1);
        shader->setUniform2f(UniformType::TextureSize, size);
        geom->drawTriangles();
        first.unbind();
    }
    {
        second.bind();
        shader->setTexture2d(UniformType::Texture, 0, first.color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 0);
        geom->drawTriangles();
        second.unbind();
    }
}

void BlurNode::onRender(RenderContext& ctx) {
    auto mainFBO = ctx.mainFBO;
    auto firstFBO = ctx.exraFBOs[0];
    auto secondFBO = ctx.exraFBOs[1];
    if(downScale == 1) {
        secondFBO = mainFBO;
    }

    Vec2i size = mainFBO->color0.getSize();
    Vec2i scaledSize = size / downScale;

    if(!firstFBO->resize(scaledSize)) {
        mainFBO->bind();
        return;
    }
    if(!secondFBO->resize(scaledSize)) {
        mainFBO->bind();
        return;
    }

    if(mainFBO != secondFBO) {
        glViewport(0, 0, scaledSize.x, scaledSize.y);
        fboCopyLogic.copy(*mainFBO, *secondFBO);
    }

    shader->bind();
    for(int i = 0; i < passes; ++i) {
        blurPass(*firstFBO, *secondFBO);
    }
    shader->unbind();

    if(mainFBO != secondFBO) {
        glViewport(0, 0, size.x, size.y);
        fboCopyLogic.copy(*secondFBO, *mainFBO);
    }

    mainFBO->bind();
}