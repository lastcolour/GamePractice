#include "Commands/DrawTexturedQuadExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"

#include <cassert>

DrawTexturedQuadExecutor::DrawTexturedQuadExecutor() {
}

DrawTexturedQuadExecutor::~DrawTexturedQuadExecutor() {
}

bool DrawTexturedQuadExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "simple_image");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Sqaure_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawTexturedQuadExecutor::deinit() {
}

void DrawTexturedQuadExecutor::registerCmdForDraw(DrawCmd* cmd) {
    DrawCommandExecutor<DrawTexturedQuadCmd>::registerCmdForDraw(cmd);
    auto drawQuadCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
    switch(drawQuadCmd->imageType) {
        case DrawTexturedQuadCmd::EImageCmdType::Image: {
            drawQuadCmd->texObj = RenderUtils::CreateTexture(drawQuadCmd->texInfo, ETextureDataType::RGBA);
            break;
        }
        case DrawTexturedQuadCmd::EImageCmdType::NinePatch: {
            drawQuadCmd->texObj = RenderUtils::CreateTexture(drawQuadCmd->texInfo, ETextureDataType::RGBA);
            break;
        }
        case DrawTexturedQuadCmd::EImageCmdType::Gradient: {
            ET_SendEventReturn(drawQuadCmd->texObj, &ETRenderTextureManager::ET_createTexture, ETextureDataType::RGBA);
            if(drawQuadCmd->texObj) {
                drawQuadCmd->texObj->bind();
                drawQuadCmd->texObj->resize(Vec2i(2));
                drawQuadCmd->texObj->setLerpType(ETextureLerpType::Point, ETextureLerpType::Point);
                drawQuadCmd->texObj->setWrapType(ETextureWrapType::ClampToEdge, ETextureWrapType::ClampToEdge);
                drawQuadCmd->texObj->unbind();
            }
        }
        default: {
            assert(false && "Ivalid cmd image type");
        }
    }
}

void DrawTexturedQuadExecutor::preDraw() {
    if(Core::EnumFlagsBitAND(events, EDrawCmdEventType::Reorder)) {
        visibleCount = DrawCmdUtils::SortCmdDrawQueue(queue);
    }
}

void DrawTexturedQuadExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    geom->bind();
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
        renderState.startCommand(cmd);

        shader->setTexture2d(UniformType::Texture, 0, *cmd.texObj, cmd.texInfo);
        shader->setUniformMat4(UniformType::ModelMat, cmd.modelMat);
        shader->setUniform1f(UniformType::Alpha, cmd.alpha);
        shader->setUniform4f(UniformType::Color, cmd.texInfo.tintColor);

        geom->drawTriangles(cmd.vertStart, cmd.vertEnd);
    }

    shader->unbind();
    geom->unbind();
}