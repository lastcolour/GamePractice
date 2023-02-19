#include "Commands/ETDrawCommands.hpp"
#include "Commands/DrawCommandExecutor.hpp"
#include "Math/MatrixTransform.hpp"
#include "Render/ETRenderManager.hpp"

#include <cassert>

namespace DrawCmdUtils {

const char* GetNameOfDrawCmdType(EDrawCmdType cmdType) {
    const char* resName = "Unknown";
    switch(cmdType) {
        case EDrawCmdType::Text: {
            resName = "Text";
            break;
        }
        case EDrawCmdType::TexturedQuad: {
            resName = "TexturedQuad";
            break;
        }
        case EDrawCmdType::Quad: {
            resName = "Quad";
            break;
        }
        case EDrawCmdType::Particles: {
            resName = "Particles";
            break;
        }
        case EDrawCmdType::Blur: {
            resName = "Blur";
            break;
        }
        case EDrawCmdType::NinePatch: {
            resName = "NinePatch";
            break;
        }
        default: {
            assert(false && "Invalid name");
        }
    }
    return resName;
}

} // namespace DrawCmdUtils


void DrawCmd::updateBlendOpPair() {
    if(!autoUpdateBlenOp) {
        return;
    }

    if(alpha >= 0.99999f) {
        blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::None, false);
    } else {
        blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::Normal, false);
    }
}

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

void DrawCmd::QueueModelMatUpdate(DrawCmd& drawCmd, const Mat4& newModelMat, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, modelMat=newModelMat](BaseDrawCommandExectuor* ex){
        cmd->modelMat = modelMat;
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
        cmd->updateBlendOpPair();
    }, cmdType);
}

void DrawCmd::QueueScaleUpdate(DrawCmd& drawCmd, const Vec3& newScale, EDrawCmdType cmdType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, scale=newScale](BaseDrawCommandExectuor* ex){
        Math::AddScale3D(cmd->modelMat, scale);
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

void DrawTextCmd::updateTextMetric() {
    if(font) {
        textMetric = font->getTextMetric(text);
    } else {
        textMetric.reset();
    }
}

void DrawTextCmd::QueueTextUpdate(DrawCmd& drawCmd, const std::string& newText) {
    std::string copyText = newText;
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, text=std::move(copyText)](BaseDrawCommandExectuor* ex){
        auto textCmd = static_cast<DrawTextCmd*>(cmd);
        textCmd->text = std::move(text);
        textCmd->updateTextMetric();
        ex->addEvent(EDrawCmdEventType::UpdateVertexData);
    }, EDrawCmdType::Text);
}

void DrawTextCmd::QueueColorUpdate(DrawCmd& drawCmd, const ColorB& newColor) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, color=newColor](BaseDrawCommandExectuor* ex){
        static_cast<DrawTextCmd*>(cmd)->color = color;
    }, EDrawCmdType::Text);
}

void DrawTextCmd::QueueFontTypeUpdate(DrawCmd& drawCmd, EFontType newFontType) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, fontType=newFontType](BaseDrawCommandExectuor* ex){
        auto textCmd = static_cast<DrawTextCmd*>(cmd);

        int prevFontHeight = -1;
        if(textCmd->font) {
            prevFontHeight = textCmd->font->getHeight();
        }

        ET_SendEventReturn(textCmd->font,
            &ETRenderFontManager::ET_createFont, fontType);

        textCmd->updateTextMetric();

        if(textCmd->font && prevFontHeight > 1.f) {
            const int newFontHeight = textCmd->font->getHeight();
            const float scale = static_cast<float>(newFontHeight) / prevFontHeight;
            Math::AddScale3D(cmd->modelMat, Vec3(scale, scale, 1.f));
        }

        if(DrawTextCmd::IsVisible(*textCmd)) {
            ex->addEvent(EDrawCmdEventType::UpdateVertexData);
        }
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
        if(color.a != 255) {
            cmd->blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::Normal, false);
        } else {
            cmd->updateBlendOpPair();
        }
    }, EDrawCmdType::Quad);
}

void DrawTexturedQuadCmd::updateTexture(const TextureInfo& newTexInfo) {
    texInfo = newTexInfo;
    texObj = RenderUtils::CreateTexture(texInfo, ETextureDataType::RGBA);
}

void DrawTexturedQuadCmd::QueueTexInfoUpdate(DrawCmd& drawCmd, const TextureInfo& newTexInfo) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, texInfo=newTexInfo](BaseDrawCommandExectuor* ex){
        auto texCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
        bool wasVisible = DrawTexturedQuadCmd::IsVisible(*texCmd);

        texCmd->updateTexture(texInfo);

        if(wasVisible && !DrawTexturedQuadCmd::IsVisible(*texCmd)) {
            ex->addEvent(EDrawCmdEventType::Reorder);
        }
    }, EDrawCmdType::TexturedQuad);
}

void DrawNinePatchCmd::QueueModelMatUpdate(DrawCmd& drawCmd, const Mat4& newModelMat) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, modelMat=newModelMat](BaseDrawCommandExectuor* ex){
        auto ninePatchesCmd = static_cast<DrawNinePatchCmd*>(cmd);
        ninePatchesCmd->modelMat = modelMat;

        ninePatchesCmd->updateVertCoords();

        if(DrawTexturedQuadCmd::IsVisible(*ninePatchesCmd)) {
            ex->addEvent(EDrawCmdEventType::UpdateVertexData);
        }
    }, EDrawCmdType::NinePatch);
}

void DrawNinePatchCmd::QueueScaleUpdate(DrawCmd& drawCmd, const Vec3& newScale) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, scale=newScale](BaseDrawCommandExectuor* ex){
        auto ninePatchesCmd = static_cast<DrawNinePatchCmd*>(cmd);
        Math::AddScale3D(ninePatchesCmd->modelMat, scale);

        ninePatchesCmd->updateVertCoords();

        if(DrawTexturedQuadCmd::IsVisible(*ninePatchesCmd)) {
            ex->addEvent(EDrawCmdEventType::UpdateVertexData);
        }
    }, EDrawCmdType::NinePatch);
}

void DrawNinePatchCmd::QueueSizeUpdate(DrawCmd& drawCmd, const Vec2& prevSize, const Vec2& newSize) {
    Vec3 scaleChange(newSize.x / prevSize.x, newSize.y / prevSize.y, 1.f);
    DrawNinePatchCmd::QueueScaleUpdate(drawCmd, scaleChange);
}

void DrawNinePatchCmd::QueueNinePatchUpdate(DrawCmd& drawCmd, const Vec2& newPatches) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, patches=newPatches](BaseDrawCommandExectuor* ex){
        auto ninePatchesCmd = static_cast<DrawNinePatchCmd*>(cmd);
        ninePatchesCmd->ninePatches = patches;
        ninePatchesCmd->updateVertCoords();

        if(DrawTexturedQuadCmd::IsVisible(*ninePatchesCmd)) {
            ex->addEvent(EDrawCmdEventType::UpdateVertexData);
        }
    }, EDrawCmdType::NinePatch);
}

void DrawNinePatchCmd::QueueTexInfoUpdate(DrawCmd& drawCmd, const TextureInfo& newTexInfo) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, texInfo=newTexInfo](BaseDrawCommandExectuor* ex){
        auto ninePatchesCmd = static_cast<DrawNinePatchCmd*>(cmd);
        bool wasVisible = DrawTexturedQuadCmd::IsVisible(*ninePatchesCmd);

        ninePatchesCmd->updateTexture(texInfo);
        ninePatchesCmd->updateVertCoords();

        if(wasVisible && !DrawTexturedQuadCmd::IsVisible(*ninePatchesCmd)) {
            ex->addEvent(EDrawCmdEventType::Reorder);
        }
    }, EDrawCmdType::NinePatch);
}

void DrawNinePatchCmd::updateVertCoords() {
    if(!texObj) {
        return;
    }

    const Vec3 scale = Math::GetScale3D(modelMat);
    const Vec2 drawSize{scale.x * 2.f, scale.y * 2.f};
    vertCoord = RenderUtils::GetNinePatchVertexCoord(texObj->getSize(), drawSize, ninePatches);
}

void DrawTexturedQuadCmd::QueueSetTexGradient(DrawCmd& drawCmd, const ColorB& startCol, const ColorB& endCol, bool isVertical) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, sCol=startCol, eCol=endCol, vertical=isVertical]
        (BaseDrawCommandExectuor* ex) {
        auto texDrawCmd = static_cast<DrawTexturedQuadCmd*>(cmd);
        if(!texDrawCmd->texObj) {
            ET_SendEventReturn(texDrawCmd->texObj, &ETRenderTextureManager::ET_createTexture, ETextureDataType::RGBA, Vec2i(2));
            if(!texDrawCmd->texObj) {
                LogError("[DrawTexturedQuadCmd::QueueSetTexGradient] Can't create texutre for an entity: '%s'",
                    EntityUtils::GetEntityName(cmd->refEntityId));
            } else {
                texDrawCmd->texInfo.lerpType = ETextureLerpType::Point;
                texDrawCmd->texInfo.wrapType = ETextureWrapType::ClampToEdge;
            }
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

        if(auto errStr = RenderUtils::GetGLError()) {
            LogError("[DrawTexturedQuadCmd::QueueSetTexGradient] Can't write to tex. (Error: '%s')",
                errStr);
        }

        if(sCol.a < 255 || eCol.a < 255) {
            texDrawCmd->blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::Normal, false);
        } else {
            texDrawCmd->blendOpPair = RenderUtils::GetBlendOpPair(EBlendMode::None, false);
        }
    }, EDrawCmdType::TexturedQuad);
}

void DrawParticlesCmd::updateTexture(const TextureInfo& newTexInfo) {
    texObj = RenderUtils::CreateTexture(newTexInfo, ETextureDataType::RGBA);
}

void DrawParticlesCmd::QueueTMUpdate(DrawCmd& drawCmd, const Transform& newTm) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, tm=newTm](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->tm = tm;
    }, EDrawCmdType::Particles);
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
        particlesCmd->updateTexture(particlesCmd->renderConfig.textureInfo);

        bool preMultAlpha = true;
        particlesCmd->blendOpPair = RenderUtils::GetBlendOpPair(renderConf.blendMode, preMultAlpha);
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

void DrawParticlesCmd::QueueStopTrackedEmitterUpdate(DrawCmd& drawCmd, EntityId trackEntId) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, entId=trackEntId](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        particlesCmd->emittersPool.stopTrackEmitter(entId);
    }, EDrawCmdType::Particles);
}

void DrawParticlesCmd::QueueStopEmitting(DrawCmd& drawCmd, bool forced) {
    ET_QueueEvent(&ETDrawCommandsManager::ET_scheduleDrawCmdEvent, [cmd=&drawCmd, forcedFlag=forced](BaseDrawCommandExectuor* ex){
        auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
        if(forcedFlag) {
            particlesCmd->emittersPool.destroyAll();
        } else {
            particlesCmd->emittersPool.stopEmittingAll();
        }
    }, EDrawCmdType::Particles);
}