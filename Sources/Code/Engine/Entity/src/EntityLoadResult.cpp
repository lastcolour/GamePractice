#include "Entity/EntityLoadResult.hpp"

#include <cassert>

EntityLoadResult::EntityLoadResult() {
}

EntityLoadResult::~EntityLoadResult() {
    if(entityId.isValid()) {
        LogError("[EntityLoadResult::~EntityLoadResult] No-one handles load result of entity: '%s'",
            EntityUtils::GetEntityName(entityId));
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, entityId);
        for(auto finishEntId : waitFinishEntities) {
            ET_SendEvent(&ETEntityManager::ET_destroyEntity, finishEntId);
        }
    } else {
        assert(waitFinishEntities.empty() && "Invalid wait result");
    }
}

void EntityLoadResult::addEntityToFinish(EntityId finishEntId) {
    assert(finishEntId.isValid() && "Invalid entity id");
    waitFinishEntities.push_back(finishEntId);
}

void EntityLoadResult::setResultEntity(EntityId resultEntId) {
    assert(!entityId.isValid() && "Invalid entity id");
    entityId = resultEntId;
    if(!resultEntId.isValid()) {
        for(auto finishEntId : waitFinishEntities) {
            ET_SendEvent(&ETEntityManager::ET_destroyEntity, finishEntId);
        }
    }
    waitFinishEntities.clear();
}

EntityId EntityLoadResult::finishLoad() {
    auto resEntId = entityId;
    entityId = InvalidEntityId;

    if(!resEntId.isValid()) {
        assert(waitFinishEntities.empty() && "Invalid wait results");
        return resEntId;
    }

    bool loadValid = true;

    for(auto finishEntId : waitFinishEntities) {
        bool finishRes = false;
        ET_SendEventReturn(finishRes, &ETEntityManager::ET_finishEntity, finishEntId);
        if(!finishRes) {
            LogError("[EntityLoadResult::finishLoad] Can't finish intermidiate load entity: '%s'",
                EntityUtils::GetEntityName(finishEntId));
            loadValid = false;
            break;
        }
    }

    if(loadValid) {
        bool finishRes = false;
        ET_SendEventReturn(finishRes, &ETEntityManager::ET_finishEntity, resEntId);
        if(!finishRes) {
            loadValid = false;
            LogError("[EntityLoadResult::finishLoad] Can't finish result entity: '%s'",
                EntityUtils::GetEntityName(resEntId));
        }
    }

    if(loadValid) {
        waitFinishEntities.clear();
        return resEntId;
    }

    ET_SendEvent(&ETEntityManager::ET_destroyEntity, resEntId);
    for(auto finishEntId : waitFinishEntities) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, finishEntId);
    }
    waitFinishEntities.clear();
    return InvalidEntityId;
}