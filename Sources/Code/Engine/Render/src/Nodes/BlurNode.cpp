#include "Nodes/BlurNode.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderTexture.hpp"
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

    fbo.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.color0.texId, 0);
    fbo.unbind();
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
    Vec2i size = mainFBO->color0.getSize();
    Vec2i scaledSize = size / downScale;

    auto firstFBO = ctx.exraFBOs[0];
    auto secondFBO = ctx.exraFBOs[1];

    resizeFBOTexture(*firstFBO, scaledSize);
    resizeFBOTexture(*secondFBO, scaledSize);

    RenderUtils::BlitFromFBOtoFBO(*mainFBO, *firstFBO);

    glViewport(0, 0, scaledSize.x, scaledSize.y);
    for(int i = 0; i < passes; ++i) {
        blurPass(*secondFBO, *firstFBO);
    }

    RenderUtils::BlitFromFBOtoFBO(*firstFBO, *mainFBO);

    glViewport(0, 0, size.x, size.y);
    mainFBO->bind();
}