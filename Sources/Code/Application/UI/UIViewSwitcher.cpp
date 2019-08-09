#include "UI/UIViewSwitcher.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <algorithm>

UIViewSwitcher::UIViewSwitcher() :
    swtichDuration(5.f) {
}

UIViewSwitcher::~UIViewSwitcher() {
}

bool UIViewSwitcher::init() {
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETUIViewSwitcher>::connect(getEntityId());
    return true;
}

void UIViewSwitcher::deinit() {
}

void UIViewSwitcher::ET_onTick(float dt) {
    if(!activeTask) {
        return;
    }

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    activeTask->duration += dt;
    float prog = std::min(activeTask->duration / swtichDuration, 1.0f);
    float shift = Math::Lerp(0.f, static_cast<float>(renderPort.x), prog);

    Transform tm;
    ET_SendEventReturn(tm, activeTask->newViewId, &ETGameObject::ET_getTransform);
    tm.pt.x = activeTask->startTm.pt.x + renderPort.x - shift;
    ET_SendEvent(activeTask->newViewId, &ETGameObject::ET_setTransform, tm);

    ET_SendEventReturn(tm, activeTask->oldViewId, &ETGameObject::ET_getTransform);
    tm.pt.x = activeTask->startTm.pt.x - shift;
    ET_SendEvent(activeTask->oldViewId, &ETGameObject::ET_setTransform, tm);

    if(activeTask->duration >= swtichDuration) {
        ET_SendEvent(&ETUIViewSwitcherEvents::ET_onViewSwitchedOut, activeTask->oldViewId);
        activeTask.reset();
    }
}

float UIViewSwitcher::ET_getSwitchDuration() const {
    return swtichDuration;
}

void UIViewSwitcher::ET_swtichView(EntityId newViewId, EntityId oldViewId) {
    if(activeTask) {
        LogError("[UIViewSwitcher::ET_swtichView] Try switch view while alreay exist active task");
        return;
    }

    activeTask.reset(new SwtichTask);
    activeTask->duration = 0.f;
    activeTask->newViewId = newViewId;
    activeTask->oldViewId = oldViewId;
    ET_SendEventReturn(activeTask->startTm, activeTask->oldViewId,
        &ETGameObject::ET_getTransform);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    ET_SendEventReturn(tm, activeTask->newViewId, &ETGameObject::ET_getTransform);
    tm.pt.x += static_cast<float>(renderPort.x);
    ET_SendEvent(activeTask->newViewId, &ETGameObject::ET_setTransform, tm);
}