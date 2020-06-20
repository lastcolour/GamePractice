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

void EditEntityTracker::ET_onRenderPortResized() {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    for(auto entityId : entities) {
        ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);
        tm.pt.x = renderPort.x / 2.f;
        tm.pt.y = renderPort.y / 2.f;

        ET_SendEvent(entityId, &ETEntity::ET_setTransform, tm);
    }
}

void EditEntityTracker::ET_startTrackingEntity(EntityId entityId) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    ET_SendEventReturn(tm, entityId, &ETEntity::ET_getTransform);
    tm.pt.x = renderPort.x / 2.f;
    tm.pt.y = renderPort.y / 2.f;

    ET_SendEvent(entityId, &ETEntity::ET_setTransform, tm);

    entities.insert(entityId);
}

void EditEntityTracker::ET_stopTrackingEntity(EntityId entityId) {
    auto it = entities.find(entityId);
    if(it != entities.end()) {
        entities.erase(it);
    }
}