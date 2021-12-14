#include "Commands/ETDrawCommands.hpp"
#include "Commands/DrawCommandExecutor.hpp"
#include "Math/MatrixTransform.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"

#include <cassert>

namespace DrawCmdUtils {

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale) {
    auto resMat = tm.toMat4();
    Math::AddScale(resMat, scale / 2.f);
    return resMat;
}

} // namespace DrawCmdUtils

void DrawCmd::QueueToRemove(DrawCmd& drawCmd, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd](BaseDrawCommandExectuor* ex){
        ex->destroyCmd(cmd);
    }, cmdType);
}

void DrawCmd::QueueToRender(DrawCmd& drawCmd, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd](BaseDrawCommandExectuor* ex){
        ex->registerCmdForDraw(cmd);
    }, cmdType);
}

void DrawCmd::QueueTMUpdate(DrawCmd& drawCmd, const Transform& newTm, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, tm=newTm](BaseDrawCommandExectuor* ex){
        cmd->modelMat = DrawCmdUtils::CalcModelMat(tm, Vec3(1.f));
    }, cmdType);
}

void DrawCmd::QueueVisUpdate(DrawCmd& drawCmd, bool newVisible, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, visible=newVisible](BaseDrawCommandExectuor* ex){
        cmd->visible = visible;
        ex->addEvent(EDrawCmdEventType::Reorder);
    }, cmdType);
}

void DrawCmd::QueueZIndexUpdate(DrawCmd& drawCmd, int newZIndex, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, zIndex=newZIndex](BaseDrawCommandExectuor* ex){
        cmd->zIndex = zIndex;
        ex->addEvent(EDrawCmdEventType::Reorder);
    }, cmdType);
}

void DrawCmd::QueueAlphaUpdate(DrawCmd& drawCmd, float newAlpha, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, alpha=newAlpha](BaseDrawCommandExectuor* ex){
        cmd->alpha = alpha;
    }, cmdType);
}

void DrawCmd::QueueScaleUpdate(DrawCmd& drawCmd, const Vec3& newScale, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, scale=newScale](BaseDrawCommandExectuor* ex){
        Math::AddScale(cmd->modelMat, scale);
    }, cmdType);
}

void DrawCmd::QueueSizeUpdate(DrawCmd& drawCmd, const Vec2& prevSize, const Vec2& newSize, EDrawCmdType cmdType) {
    Vec3 scaleChange(newSize.x / prevSize.x, newSize.y / prevSize.y, 1.f);
    DrawCmd::QueueScaleUpdate(drawCmd, scaleChange, cmdType);
}

void DrawCmd::QueueStencilDataUpdate(DrawCmd& drawCmd, const StencilWirteReadData& newStencilData, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, stencilData=newStencilData](BaseDrawCommandExectuor* ex){
        cmd->stencilData = stencilData;
    }, cmdType);
}

void DrawTextCmd::QueueTextUpdate(DrawCmd& drawCmd, const std::string& newText) {
    std::string copyText = newText;
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, text=std::move(copyText)](BaseDrawCommandExectuor* ex){
        static_cast<DrawTextCmd*>(cmd)->text = std::move(text);
        ex->addEvent(EDrawCmdEventType::UpdateVertexData);
    }, EDrawCmdType::Text);
}

void DrawTextCmd::QueueColorUpdate(DrawCmd& drawCmd, const ColorB& newColor) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, color=newColor](BaseDrawCommandExectuor* ex){
        static_cast<DrawTextCmd*>(cmd)->color = color;
    }, EDrawCmdType::Text);
}

void DrawTextCmd::QueueFontHeightUpdate(DrawCmd& drawCmd, float newFontHeight) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, fontHeight=newFontHeight](BaseDrawCommandExectuor* ex){
        static_cast<DrawTextCmd*>(cmd)->fontHeight = fontHeight;
    }, EDrawCmdType::Text);
}

void DrawTextCmd::QueueFontTypeUpdate(DrawCmd& drawCmd, EFontType newFontType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, fontType=newFontType](BaseDrawCommandExectuor* ex){
        ET_SendEventReturn(static_cast<DrawTextCmd*>(cmd)->font,
            &ETRenderFontManager::ET_createFont, fontType);
    }, EDrawCmdType::Text);
}

void DrawTextCmd::QueueAlignUpdate(DrawCmd& drawCmd, bool newAlign) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, align=newAlign](BaseDrawCommandExectuor* ex){
        static_cast<DrawTextCmd*>(cmd)->alignAtCenter = align;
    }, EDrawCmdType::Text);
}

void DrawColoredQuadCmd::QueueColorUpdate(DrawCmd& drawCmd, const ColorB& newColor) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, color=newColor](BaseDrawCommandExectuor* ex){
        static_cast<DrawColoredQuadCmd*>(cmd)->color = color;
    }, EDrawCmdType::Quad);
}

void DrawTexturedQuadCmd::QueueTexInfoUpdate(DrawCmd& drawCmd, const TextureInfo& newTexInfo) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, texInfo=newTexInfo](BaseDrawCommandExectuor* ex){
        auto texCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
        bool wasVisible = DrawTexturedQuadCmd::IsVisible(*texCmd);

        texCmd->texInfo = texInfo;
        texCmd->texObj = RenderUtils::CreateTexture(texInfo, ETextureDataType::RGBA);

        if(wasVisible && !DrawTexturedQuadCmd::IsVisible(*texCmd)) {
            ex->addEvent(EDrawCmdEventType::Reorder);
        }
    }, EDrawCmdType::TexturedQuad);
}

void DrawTexturedQuadCmd::QueueNinePatchUpdate(DrawCmd& drawCmd, const Vec2& newPatches) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, patches=newPatches](BaseDrawCommandExectuor* ex){
        static_cast<DrawTexturedQuadCmd*>(cmd)->ninePatches = patches;
        ex->addEvent(EDrawCmdEventType::UpdateVertexData);
    }, EDrawCmdType::TexturedQuad);
}

void DrawTexturedQuadCmd::QueueSetTexGradient(DrawCmd& drawCmd, const ColorB& startCol, const ColorB& endCol, bool isVertical) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, sCol=startCol, eCol=endCol, vertical=isVertical]
        (BaseDrawCommandExectuor* ex) {
        auto texDrawCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
        if(!texDrawCmd->texObj) {
            return;
        }
        texDrawCmd->texObj->bind();
        if(vertical) {
            ColorB vertColors[] = {
                sCol,
                sCol,
                eCol,
                eCol
            };
            texDrawCmd->texObj->writeRGBA(Vec2i(0), Vec2i(2), vertColors);
        } else {
            ColorB vertColors[] = {
                sCol,
                eCol,
                sCol,
                eCol
            };
            texDrawCmd->texObj->writeRGBA(Vec2i(0), Vec2i(2), vertColors);
        }
        texDrawCmd->texObj->unbind();

        if(sCol.a < 255 || eCol.a < 255) {
            texDrawCmd->blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::Normal, false);
        } else {
            texDrawCmd->blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::None, false);
        }
    }, EDrawCmdType::TexturedQuad);
}

void DrawParticlesCmd::QueueColorConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterColorConfig& newColorConf) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, colorConf=newColorConf](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.getSimConfig().color = colorConf;
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueMovementConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterMovementConfig& newMovementConf) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, movementConf=newMovementConf](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.getSimConfig().movement = movementConf;
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueEmissionConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterEmissionConfig& newEmissionConf) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, emissionConf=newEmissionConf](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.getSimConfig().emission = emissionConf;
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueRenderConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterRenderConfig& newRenderConf) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, renderConf=newRenderConf](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->renderConfig = renderConf;
        particlesCmd->texObj = RenderUtils::CreateTexture(renderConf.textureInfo, ETextureDataType::RGBA);
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueSubEmittersConfigUpdate(DrawCmd& drawCmd, const ParticlesSubEmittersConfig& newSubEmittersConf) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, sumEmittersConf=newSubEmittersConf](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.getSimConfig().subEmittorsConfig = sumEmittersConf;
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueSizeConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterSizeConfig& newSizeConf) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, sizeConf=newSizeConf](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.getSimConfig().sizeConfig = sizeConf;
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueCreateEmitterUpdate(DrawCmd& drawCmd, const EmitRequest& emitReq) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, req=emitReq](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.createEmitter(req);
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueStopTrackedEmitterUpate(DrawCmd& drawCmd, EntityId trackEntId) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, entId=trackEntId](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.stopTrackEmitter(entId);
    }, EDrawCmdType::Particles);
}