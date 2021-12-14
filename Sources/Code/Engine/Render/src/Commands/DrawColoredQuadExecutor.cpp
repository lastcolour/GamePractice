#include "Commands/DrawColoredQuadExecutor.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

DrawColoredQuadExecutor::DrawColoredQuadExecutor() {
}

DrawColoredQuadExecutor::~DrawColoredQuadExecutor() {
}

bool DrawColoredQuadExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "geom_solid_color");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Square);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawColoredQuadExecutor::deinit() {
}

void DrawColoredQuadExecutor::preDraw() {
    if(Core::EnumFlagsBitAND(events, EDrawCmdEventType::Reorder)) {
        visibleCount = DrawCmdUtils::SortCmdDrawQueue(queue);
    }
}

void DrawColoredQuadExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    geom->bind();
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
        renderState.startCommand(cmd);

        shader->setUniformMat4(UniformType::ModelMat, cmd.modelMat);
        // shader->setUniform4f(UniformType::Color,
        //     ColorB(cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a * cmd.alpha));
        geom->drawTriangles(0, 6);
    }

    shader->unbind();
    geom->unbind();
}