#include "UI/UIViewSwitcher.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <algorithm>

UIViewSwitcher::UIViewSwitcher() :
    swtichDuration(0.2f) {
}

UIViewSwitcher::~UIViewSwitcher() {
}

bool UIViewSwitcher::init() {
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETUIViewSwitcher>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
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

    float shiftForNewView = renderPort.x - shift;
    float shiftForOldView = -shift;

    if(activeTask->reverse) {
        shiftForNewView = -shiftForNewView;
        shiftForOldView = -shiftForOldView;
    }

    Transform tm;
    ET_SendEventReturn(tm, activeTask->newViewId, &ETEntity::ET_getTransform);
    tm.pt.x = activeTask->startTm.pt.x + shiftForNewView;
    ET_SendEvent(activeTask->newViewId, &ETEntity::ET_setTransform, tm);

    ET_SendEventReturn(tm, activeTask->oldViewId, &ETEntity::ET_getTransform);
    tm.pt.x = activeTask->startTm.pt.x + shiftForOldView;
    ET_SendEvent(activeTask->oldViewId, &ETEntity::ET_setTransform, tm);

    if(activeTask->duration >= swtichDuration) {
        EntityId oldViewId = activeTask->oldViewId;
        activeTask.reset();
        ET_SendEvent(&ETUIViewSwitcherEvents::ET_onViewSwitchFinished, oldViewId);
    }
}

float UIViewSwitcher::ET_getSwitchDuration() const {
    return swtichDuration;
}

void UIViewSwitcher::setupTask(EntityId newViewId, EntityId oldViewId, bool reverse) {
    if(activeTask) {
        LogError("[UIViewSwitcher::setupTask] Try switch view while alreay exist active task");
        return;
    }

    activeTask.reset(new SwtichTask);
    activeTask->duration = 0.f;
    activeTask->newViewId = newViewId;
    activeTask->oldViewId = oldViewId;
    activeTask->reverse = reverse;
    ET_SendEventReturn(activeTask->startTm, activeTask->oldViewId,
        &ETEntity::ET_getTransform);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    ET_SendEventReturn(tm, activeTask->newViewId, &ETEntity::ET_getTransform);

    if(reverse) {
        tm.pt.x -= static_cast<float>(renderPort.x);
    } else {
        tm.pt.x += static_cast<float>(renderPort.x);
    }
    ET_SendEvent(activeTask->newViewId, &ETEntity::ET_setTransform, tm);
}

void UIViewSwitcher::ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) {
    setupTask(newViewId, oldViewId, true);
}

void UIViewSwitcher::ET_swtichView(EntityId newViewId, EntityId oldViewId) {
    setupTask(newViewId, oldViewId, false);
}

void UIViewSwitcher::ET_reverse() {
    if(!activeTask) {
        LogError("[UIViewSwitcher::ET_reverse] Can't reverse task without task");
        return;
    }
    activeTask->duration = swtichDuration - activeTask->duration;
    activeTask->reverse = !activeTask->reverse;
    std::swap(activeTask->newViewId, activeTask->oldViewId);
}

void UIViewSwitcher::ET_forceSwtichStop() {
    if(activeTask) {
        activeTask.reset();
    }
}

void UIViewSwitcher::ET_onRenderPortResized() {
    if(activeTask) {
        activeTask->duration = swtichDuration;
        ET_onTick(0.f);
    }
}