#include "EntityRegistry.hpp"
#include "Entity.hpp"

#include <cassert>

EntityRegistry::EntityRegistry() {
}

EntityRegistry::~EntityRegistry() {
    removeAllEntities();
}

Entity* EntityRegistry::createEntity(const char* name) {
    Entity* res = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        res = pool.create(name, this,
            GetEnv()->GetETSystem()->createNewEntityId());
        assert(res && "Can't create entity");
        searchTable[res->getEntityId()] = res;
    }
    return res;
}

Entity* EntityRegistry::findEntity(EntityId entityId) {
    Entity* res = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        auto it = searchTable.find(entityId);
        if(it != searchTable.end()) {
            res = it->second;
        }
    }
    return res;
}

void EntityRegistry::removeEntity(Entity* entity) {
    assert(entity && "Invalid entity");
    removeEntity(entity->getEntityId());
}

void EntityRegistry::removeAllEntities() {
    std::unordered_map<EntityId, Entity*> entitiesToRemove;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        entitiesToRemove = std::move(searchTable);
    }
    for(auto& entity : entitiesToRemove) {
        entity.second->purgeAllRelationships();
    }
    entitiesToRemove.clear();
}

void EntityRegistry::removeEntity(EntityId entityId) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    auto it = searchTable.find(entityId);
    if (it != searchTable.end()) {
        pool.recycle(it->second);
        searchTable.erase(it);
    }
}

size_t EntityRegistry::getEntitiesCount() const {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return searchTable.size();
}
