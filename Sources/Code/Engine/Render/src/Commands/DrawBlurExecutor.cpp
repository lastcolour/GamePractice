#include "Commands/DrawBlurExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderFramebuffer.hpp"
#include "Platform/OpenGL.hpp"

DrawBlurExecutor::DrawBlurExecutor() {
}

DrawBlurExecutor::~DrawBlurExecutor() {
}

bool DrawBlurExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "blur");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawBlurExecutor::deinit() {
}

void DrawBlurExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    renderState.mainFBO->unbind();

    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];

        BlurFBOs FBOs;
        if(!setupFBOs(cmd, renderState, FBOs)) {
            LogError("[DrawBlurExecutor::draw] Can't setup FBOs (Entity: '%s')",
                EntityUtils::GetEntityName(cmd.refEntityId));
            continue;
        }

        renderState.startCommand(cmd);

        geom->bind();
        shader->bind();
        shader->setUniform2f(UniformType::TextureSize, FBOs.second->getSize());
        for(int p = 0; p < cmd.passes; ++p) {
            blurPass(FBOs);
        }
        shader->unbind();
        geom->unbind();

        if(FBOs.main != FBOs.second) {
            const auto mainFBOsize = FBOs.main->getSize();
            glViewport(0, 0, mainFBOsize.x, mainFBOsize.y);
            renderState.copyFBOtoFBO(*FBOs.second, *FBOs.main);
        }
    }

    renderState.mainFBO->bind();
}

bool DrawBlurExecutor::setupFBOs(const DrawBlurCmd& cmd, RenderState& renderState, DrawBlurExecutor::BlurFBOs& FBOs) {
    FBOs.main = renderState.mainFBO.get();
    FBOs.first = renderState.extraFBOs[0].get();
    FBOs.second = renderState.extraFBOs[1].get();

    if(cmd.downScale == 1) {
        FBOs.second = FBOs.main;
    }

    Vec2i size = FBOs.main->getSize();
    Vec2i scaledSize = size / cmd.downScale;

    bool setupValid = true;
    {
        FBOs.first->bind();
        if(!FBOs.first->resize(scaledSize)) {
            setupValid = false;
        }
        FBOs.first->unbind();
    }

    if(setupValid) {
        FBOs.second->bind();
        if(!FBOs.second->resize(scaledSize)) {
            setupValid = false;
        }
        FBOs.second->unbind();
    }

    if(setupValid && FBOs.main != FBOs.second) {
        glViewport(0, 0, scaledSize.x, scaledSize.y);
        if(!renderState.copyFBOtoFBO(*FBOs.main, *FBOs.second)) {
            setupValid = false;
        }
    }

    return setupValid;
}

void DrawBlurExecutor::blurPass(DrawBlurExecutor::BlurFBOs& FBOs) {
    {
        FBOs.first->bind();
        shader->setTexture2d(UniformType::Texture, 0, FBOs.second->color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 1);
        geom->drawTriangles(0, 6);
        FBOs.first->unbind();
    }
    {
        FBOs.second->bind();
        shader->setTexture2d(UniformType::Texture, 0, FBOs.first->color0);
        shader->setUniform1i(UniformType::IsVerticalPass, 0);
        geom->drawTriangles(0, 6);
        FBOs.second->unbind();
    }
}