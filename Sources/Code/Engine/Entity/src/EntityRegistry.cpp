#include "EntityRegistry.hpp"
#include "Core/ETLogger.hpp"
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
        std::lock_guard<std::mutex> lock(mutex);
        std::unique_ptr<Entity> entity(new Entity(name, GetETSystem()->createNewEntityId()));
        res = entity.get();
        entities[res->getEntityId()] = std::move(entity);
    }
    return res;
}

Entity* EntityRegistry::findEntity(EntityId entityId) {
    Entity* res = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = entities.find(entityId);
        if(it != entities.end()) {
            res = it->second.get();
        }
    }
    return res;
}

void EntityRegistry::removeEntity(Entity* entity) {
    assert(entity && "Invalid entity");
    removeEntity(entity->getEntityId());
}

void EntityRegistry::removeAllEntities() {
    EntityContainerT entitiesToRemove;
    {
        std::lock_guard<std::mutex> lock(mutex);
        entitiesToRemove = std::move(entities);
    }
    entitiesToRemove.clear();
}

void EntityRegistry::removeEntity(EntityId entityId) {
    std::unique_ptr<Entity> entityToRemove;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = entities.find(entityId);
        if(it == entities.end()) {
            return;
        }
        entityToRemove = std::move(it->second);
        entities.erase(it);
    }
    entityToRemove.reset();
}
