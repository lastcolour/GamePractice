#include "EditEntityTracker.hpp"
#include "Entity/ETEntity.hpp"

EditEntityTracker::EditEntityTracker() {
}

EditEntityTracker::~EditEntityTracker() {
}

bool EditEntityTracker::init() {
    ETNode<ETRenderCameraEvents>::connect(getEntityId());
    ETNode<ETEditEntityTracker>::connect(getEntityId());
    return true;
}

void EditEntityTracker::deinit() {
}

void EditEntityTracker::ET_onRenderPortResized(const Vec2i& newSize) {
    Transform tm;
    for(auto trackEntId : entities) {
        ET_SendEventReturn(tm, trackEntId, &ETEntity::ET_getTransform);
        tm.pt.x = newSize.x / 2.f;
        tm.pt.y = newSize.y / 2.f;

        ET_SendEvent(trackEntId, &ETEntity::ET_setTransform, tm);
    }
}

void EditEntityTracker::ET_startTrackingEntity(EntityId trackEntId) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    ET_SendEventReturn(tm, trackEntId, &ETEntity::ET_getTransform);
    tm.pt.x = renderPort.x / 2.f;
    tm.pt.y = renderPort.y / 2.f;

    ET_SendEvent(trackEntId, &ETEntity::ET_setTransform, tm);

    entities.insert(trackEntId);
}

void EditEntityTracker::ET_stopTrackingEntity(EntityId trackEntId) {
    auto it = entities.find(trackEntId);
    if(it != entities.end()) {
        entities.erase(it);
    }
}