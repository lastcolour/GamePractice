#include "Entity/EntityLoadResult.hpp"
#include "Entity/ETEntity.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

EntityLoadResult::EntityLoadResult(EntityId loadedEntId) :
    entityId(loadedEntId) {
}

EntityLoadResult::EntityLoadResult(const EntityLoadResult&& other) :
    entityId(other.entityId) {
}

EntityLoadResult& EntityLoadResult::operator=(EntityLoadResult&& other) {
    if(entityId.isValid()) {
        assert(false && "Invalid opearation");
        LogError("[EntityLoadResult::operator=] Lost result of entity load: '%s'", EntityUtils::GetEntityName(entityId));
    }
    entityId = other.entityId;
    other.entityId = InvalidEntityId;
    return *this;
}

EntityLoadResult::~EntityLoadResult() {
    if(entityId.isValid()) {
        LogError("[EntityLoadResult::~EntityLoadResult] No-one handles load result of entity: '%s'", EntityUtils::GetEntityName(entityId));
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, entityId);
    }
}

EntityId EntityLoadResult::finishLoad() {
    auto resEntId = entityId;
    entityId = InvalidEntityId;

    if(!resEntId.isValid()) {
        return resEntId;
    }

    bool finishRes = false;
    ET_SendEventReturn(finishRes, &ETEntityManager::ET_finishEntity, resEntId);
    if(finishRes) {
        return resEntId;
    }

    ET_SendEvent(&ETEntityManager::ET_destroyEntity, resEntId);
    return InvalidEntityId;
}