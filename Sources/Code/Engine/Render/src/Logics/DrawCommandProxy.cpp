#include "Logics/DrawCommandProxy.hpp"
#include "Commands/ETDrawCommands.hpp"

#include <cassert>

void DrawCommandProxy::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<DrawCommandProxy>("DrawCommandProxy")) {
        classInfo->addField("isVisible", &DrawCommandProxy::isVisible);
        classInfo->addField("alpha", &DrawCommandProxy::alpha);
        classInfo->addField("zIndex", &DrawCommandProxy::zIndex);
    }
}

DrawCommandProxy::DrawCommandProxy(EDrawCmdType drawCmdType) :
    cmd(nullptr),
    alpha(1.f),
    alphaMult(1.f),
    normScale(1.f),
    zIndex(0),
    cmdType(drawCmdType),
    isVisible(true),
    isLoaded(false),
    emitEvents(false) {
}

DrawCommandProxy::~DrawCommandProxy() {
}

DrawCmd* DrawCommandProxy::getDrawCmd() {
    return cmd;
}

const DrawCmd* DrawCommandProxy::getDrawCmd() const {
    return cmd;
}

void DrawCommandProxy::init() {
    ET_SendEventReturn(cmd, &ETDrawCommandsManager::ET_createDrawCmd, cmdType);
    if(!cmd) {
        LogError("[DrawCommandProxy::init] Can't create draw cmd of type '%s' for an enity: '%s'",
            DrawCmdUtils::GetNameOfDrawCmdType(cmdType), getEntityName());
        return;
    }

    alpha = Math::Clamp(alpha, 0.f, 1.f);

    cmd->refEntityId = getEntityId();
    cmd->alpha = alpha * alphaMult;
    cmd->zIndex = zIndex;
    cmd->modelMat = calcModelMat();

    onInit();

    DrawCmd::QueueToRender(*cmd, cmdType);

    ETNode<ETRenderNode>::connect(getEntityId());
}

void DrawCommandProxy::deinit() {
    isLoaded = false;
    normScale = 1.f;
    stencilData = StencilWirteReadData{};
    alphaMult = 1.f;
    ETNode<ETRenderNode>::disconnect();
    if(cmd != nullptr) {
        DrawCmd::QueueToRemove(*cmd, cmdType);
        cmd = nullptr;
    }
}

void DrawCommandProxy::ET_setAlphaMultiplier(float newAlphaMult) {
    alphaMult = newAlphaMult;
    if(alphaMult < -0.f || alphaMult > 1.f) {
        LogWarning("[DrawCommandProxy::ET_setAlphaMultiplier] alpha-multiplier '%.1f' is out of range [0..1] (Entity: '%s')",
            alphaMult, getEntityName());
        alphaMult = Math::Clamp(alphaMult, 0.f, 1.f);
    }
    float resAlpha = Math::Clamp(alphaMult * alpha, 0.f, 1.f);
    DrawCmd::QueueAlphaUpdate(*cmd, resAlpha, cmdType);
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onAlphaMultChanged, newAlphaMult);
    }
}

float DrawCommandProxy::ET_getAlphaMultiplier() const {
    return alphaMult;
}

bool DrawCommandProxy::ET_isVisible() const {
    return isVisible;
}

void DrawCommandProxy::ET_hide() {
    if(!isVisible) {
        return;
    }
    isVisible = false;
    if(!isLoaded) {
        return;
    }
    DrawCmd::QueueVisUpdate(*cmd, isVisible, cmdType);
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onHidden, !isVisible);
    }
}

void DrawCommandProxy::ET_show() {
    if(isVisible) {
        return;
    }
    isVisible = true;
    if(!isLoaded) {
        return;
    }
    DrawCmd::QueueVisUpdate(*cmd, isVisible, cmdType);
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onHidden, !isVisible);
    }
}

void DrawCommandProxy::ET_setZIndex(int newZIndex) {
    if(newZIndex == zIndex) {
        return;
    }
    zIndex = newZIndex;
    DrawCmd::QueueZIndexUpdate(*cmd, zIndex, cmdType);
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onZIndexChanged, zIndex);
    }
}

int DrawCommandProxy::ET_getZIndex() const {
    return zIndex;
}

void DrawCommandProxy::ET_setStencilData(const StencilWirteReadData& newStencilData) {
    if(newStencilData.opType == stencilData.opType
        && newStencilData.refVal == stencilData.refVal) {
        return;
    }
    stencilData = newStencilData;
    DrawCmd::QueueStencilDataUpdate(*cmd, stencilData, cmdType);
}

void DrawCommandProxy::onTransformChanged(const Transform& newTm) {
    if(cmd) {
        const Mat4 modelMat = calcModelMat();
        DrawCmd::QueueModelMatUpdate(*cmd, modelMat, cmdType);
    }
}

void DrawCommandProxy::ET_setNormalizationScale(float newNormScale) {
    float scaleDiff = std::max(0.001f, newNormScale / normScale);
    DrawCmd::QueueScaleUpdate(*cmd, Vec3(scaleDiff, scaleDiff, 1.f), cmdType);
    normScale = newNormScale;
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onNormScaleChanged, normScale);
    }
}

float DrawCommandProxy::ET_getNormalizationScale() const {
    return normScale;
}

void DrawCommandProxy::onLoaded() {
    isLoaded = true;
    if(ET_isVisible()) {
        DrawCmd::QueueVisUpdate(*cmd, true, cmdType);
    }
}

bool DrawCommandProxy::ET_setEmitEvents(bool flag) {
    emitEvents = flag;
    return true;
}