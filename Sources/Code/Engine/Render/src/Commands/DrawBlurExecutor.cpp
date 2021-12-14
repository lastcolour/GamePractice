#include "Commands/DrawBlurExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderFramebuffer.hpp"
#include "Platform/OpenGL.hpp"

namespace {

void setupFBO() {
    /*
    if(cmd.downScale == 1) {
        secondFBO = mainFBO;
    }

    Vec2i size = mainFBO->color0.getSize();
    Vec2i scaledSize = size / cmd.downScale;

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
    */
}

void blurPass(RenderFramebuffer& first, RenderFramebuffer& second) {
    Vec2i size = second.color0.getSize();

    /*
    {
        first.bind();
        shader->setTexture2d(UniformType::Texture, 0, second.color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 1);
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
    */
}

void processCmd(const DrawBlurCmd& cmd) {

}

} // namespace

DrawBlurExecutor::DrawBlurExecutor() {
}

DrawBlurExecutor::~DrawBlurExecutor() {
}

bool DrawBlurExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "blur");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawBlurExecutor::deinit() {
}

void DrawBlurExecutor::preDraw() {
}

void DrawBlurExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    auto mainFBO = renderState.mainFBO;
    auto firstFBO = renderState.extraFBOs[0];
    auto secondFBO = renderState.extraFBOs[1];

    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
    
        setupFBO();
        shader->bind();
        for(int p = 0; p < cmd.passes; ++p) {
            blurPass(*firstFBO, *secondFBO);
        }
        shader->unbind();

        if(mainFBO != secondFBO) {
            glViewport(0, 0, size.x, size.y);
            renderState.copyFBOtoFBO(*secondFBO, *mainFBO);
        }

        mainFBO->bind();
    }
}