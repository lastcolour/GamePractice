#include "Commands/DrawTexturedQuadExecutor.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

namespace {

void setupGeometry(DrawTexturedQuadCmd& cmd, std::vector<Vec4>& out) {
    cmd.vertStart = 0;
    cmd.vertEnd = 6;
}

} // namespace

DrawTexturedQuadExecutor::DrawTexturedQuadExecutor() {
}

DrawTexturedQuadExecutor::~DrawTexturedQuadExecutor() {
}

bool DrawTexturedQuadExecutor::init() {
    ET_SendEventReturn(imgShader, &ETRenderShaderManager::ET_createShader, "simple_image");
    if(!imgShader) {
        return false;
    }
    ET_SendEventReturn(gradShader, &ETRenderShaderManager::ET_createShader, "tex_vert_color");
    if(!gradShader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawTexturedQuadExecutor::deinit() {
}

void DrawTexturedQuadExecutor::registerCmdForDraw(DrawCmd* cmd) {
    auto texQuadCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    if(texQuadCmd->imageType != DrawTexturedQuadCmd::EImageCmdType::Gradient) {
        texQuadCmd->updateTexture(texQuadCmd->texInfo);
    }
    DrawCommandExecutor<DrawTexturedQuadCmd>::registerCmdForDraw(cmd);
}

void DrawTexturedQuadExecutor::preDraw(RenderState& renderState) {
    DrawCommandExecutor<DrawTexturedQuadCmd>::preDraw(renderState);
    if(!Core::EnumFlagsBitAND(events, EDrawCmdEventType::UpdateVertexData)) {
        return;
    }
    std::vector<Vec4> vertData;
    for(size_t i = 0; i < visibleCount; ++i) {
        DrawTexturedQuadCmd* cmd = queue[i];
        setupGeometry(*cmd, vertData);
    }
}


void DrawTexturedQuadExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    RenderShader* prevShader = nullptr;

    geom->bind();
    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];

        RenderShader* currShader = nullptr;
        if(cmd.imageType == DrawTexturedQuadCmd::EImageCmdType::Gradient) {
            currShader = gradShader.get();
        } else {
            currShader = imgShader.get();
        }
        if(currShader != prevShader) {
            if(prevShader) {
                prevShader->unbind();
            }
            prevShader = currShader;
            currShader->bind();
            currShader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);
        }

        renderState.startCommand(cmd);

        currShader->setTexture2d(UniformType::Texture, 0, *cmd.texObj, cmd.texInfo);
        currShader->setUniformMat4(UniformType::ModelMat, cmd.modelMat);
        currShader->setUniform1f(UniformType::Alpha, cmd.alpha);
        currShader->setUniform4f(UniformType::Color, cmd.texInfo.tintColor);

        geom->drawTriangles(cmd.vertStart, cmd.vertEnd);
    }

    if(prevShader) {
        prevShader->unbind();
    }
    geom->unbind();
}