#include "Logics/DrawCommandProxy.hpp"
#include "Commands/ETDrawCommands.hpp"
#include "RenderUtils.hpp"

void DrawCommandProxy::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<DrawCommandProxy>("DrawCommandProxy")) {
        classInfo->addField("isVisible", &DrawCommandProxy::isVisible);
        classInfo->addField("alpha", &DrawCommandProxy::alpha);
        classInfo->addField("zIndex", &DrawCommandProxy::zIndex);
    }
}

DrawCommandProxy::DrawCommandProxy(EDrawCmdType cmdType) :
    cmd(nullptr),
    alpha(1.f),
    alphaMult(1.f),
    normScale(1.f),
    zIndex(0),
    type(cmdType),
    isVisible(true),
    isLoaded(false),
    emitEvents(false) {
}

DrawCommandProxy::~DrawCommandProxy() {
}

void DrawCommandProxy::init() {
    ET_SendEventReturn(cmd, &ETDrawCommandsManager::ET_createDrawCmd, type);
    if(!cmd) {
        LogError("[DrawCommandProxy::init] Can't create draw cmd of type '%s' for an enity: '%s'",
            RenderUtils::GetDrawCmdTypeName(type), EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.scale *= normScale;

    alpha = Math::Clamp(alpha, 0.f, 1.f);

    cmd->refEntityId = getEntityId();
    cmd->alpha = alpha * alphaMult;
    cmd->zIndex = zIndex;
    cmd->modelMat = Mat4(1.f);

    onInit();

    DrawCmd::QueueToRender(*cmd, type);

    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
}

void DrawCommandProxy::deinit() {
    zIndex = 0;
    alphaMult = 1.f;
    normScale = 1.f;
    emitEvents = false;
    isLoaded = false;
    ETNode<ETEntityEvents>::disconnect();
    ETNode<ETRenderNode>::disconnect();
    if(cmd != nullptr) {
        DrawCmd::QueueToRemove(*cmd, type);
        cmd = nullptr;
    }
}

void DrawCommandProxy::ET_setAlphaMultiplier(float newAlphaMult) {
    alphaMult = newAlphaMult;
    if(alphaMult < -0.f || alphaMult > 1.f) {
        LogWarning("[DrawCommandProxy::ET_setAlphaMultiplier] alpha-multiplier '%.1f' is out of range [0..1] (Entity: '%s')",
            alphaMult, EntityUtils::GetEntityName(getEntityId()));
        alphaMult = Math::Clamp(alphaMult, 0.f, 1.f);
    }
    float resAlpha = Math::Clamp(alphaMult * alpha, 0.f, 1.f);
    DrawCmd::QueueAlphaUpdate(*cmd, resAlpha, type);
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
    DrawCmd::QueueVisUpdate(*cmd, isVisible, type);
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
    DrawCmd::QueueVisUpdate(*cmd, isVisible, type);
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onHidden, !isVisible);
    }
}

void DrawCommandProxy::ET_setZIndex(int newZIndex) {
    if(newZIndex == zIndex) {
        return;
    }
    zIndex = newZIndex;
    DrawCmd::QueueZIndexUpdate(*cmd, zIndex, type);
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
    DrawCmd::QueueStencilDataUpdate(*cmd, stencilData, type);
}

void DrawCommandProxy::ET_onTransformChanged(const Transform& newTm) {
    DrawCmd::QueueTMUpdate(*cmd, newTm, type);
}

void DrawCommandProxy::ET_setNormalizationScale(float newNormScale) {
    float scaleDiff = std::max(0.001f, newNormScale / normScale);
    DrawCmd::QueueScaleUpdate(*cmd, Vec3(scaleDiff), type);
    normScale = newNormScale;
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onNormScaleChanged, normScale);
    }
}

float DrawCommandProxy::ET_getNormalizationScale() const {
    return normScale;
}

void DrawCommandProxy::ET_onLoaded() {
    isLoaded = true;
    if(ET_isVisible()) {
        DrawCmd::QueueVisUpdate(*cmd, true, type);
    }
}

bool DrawCommandProxy::ET_setEmitEvents(bool flag) {
    emitEvents = flag;
    return true;
}