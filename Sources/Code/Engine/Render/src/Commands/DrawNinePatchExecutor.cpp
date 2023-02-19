#include "Commands/DrawNinePatchExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"

namespace {

int setupGeometry(DrawNinePatchCmd& cmd, int vertShift, std::vector<Vert_Vec3_Tex>& out) {
    cmd.vertStart = vertShift;
    cmd.vertEnd = cmd.vertStart + RenderUtils::CreateNinePatchVertData(cmd.vertCoord, cmd.ninePatches, &out[vertShift]);
    return cmd.vertEnd;
}

} // namespace

bool DrawNinePatchExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "simple_image");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawNinePatchExecutor::deinit() {
}

void DrawNinePatchExecutor::registerCmdForDraw(DrawCmd* cmd) {
    auto ninePatchCmd = static_cast<DrawNinePatchCmd*>(cmd);
    ninePatchCmd->updateTexture(ninePatchCmd->texInfo);
    ninePatchCmd->updateVertCoords();
    DrawCommandExecutor<DrawNinePatchCmd>::registerCmdForDraw(cmd);
}

void DrawNinePatchExecutor::preDraw(RenderState& renderState) {
    DrawCommandExecutor<DrawNinePatchCmd>::preDraw(renderState);

    if(!Core::EnumFlagsBitAND(events, EDrawCmdEventType::UpdateVertexData)) {
        return;
    }

    int vertShift = 0;
    std::vector<Vert_Vec3_Tex> vertData;
    vertData.resize(visibleCount * RenderUtils::NinePatchesVertCount);

    for(int i = 0; i < visibleCount; ++i) {
        auto ninePatchCmd = static_cast<DrawNinePatchCmd*>(queue[i]);
        vertShift = setupGeometry(*ninePatchCmd, vertShift, vertData);
    }

    geom->setVboData(&vertData[0], sizeof(Vert_Vec3_Tex) * vertData.size());
}

void DrawNinePatchExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    geom->bind();
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    const DrawNinePatchCmd* prevCmd = nullptr;
    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *static_cast<DrawNinePatchCmd*>(queue[i]);
        renderState.startCommand(cmd);

        shader->setUniformMat4(UniformType::ModelMat, cmd.modelMat);

        if(!prevCmd || prevCmd->texObj.get() != cmd.texObj.get() || prevCmd->texInfo != cmd.texInfo) {
            shader->setTexture2d(UniformType::Texture, 0, *cmd.texObj, cmd.texInfo);
            shader->setUniform4f(UniformType::Color, cmd.texInfo.tintColor);
        }

        if(!prevCmd || !Math::IsEqual(prevCmd->alpha, cmd.alpha, FLT_MIN)) {
            shader->setUniform1f(UniformType::Alpha, cmd.alpha);
        }

        geom->drawTriangles(cmd.vertStart, cmd.vertEnd);

        prevCmd = &cmd;
    }

    shader->unbind();
    geom->unbind();
}