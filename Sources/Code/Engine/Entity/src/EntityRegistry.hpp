#ifndef __ENTITY_CONTEXT_HPP__
#define __ENTITY_CONTEXT_HPP__

#include "Core/Core.hpp"
#include "Math/Transform.hpp"

#include <mutex>
#include <memory>
#include <unordered_map>

class Entity;

class EntityRegistry {
public:

    EntityRegistry();
    ~EntityRegistry();

    Entity* createEntity(const char* name);
    Entity* findEntity(EntityId entityId);
    void removeEntity(Entity* entity);
    void removeEntity(EntityId entity);
    void removeAllEntities();

private:

    std::mutex mutex;
    std::unordered_map<EntityId, std::unique_ptr<Entity>> entities;
};

#endif /* __ENTITY_CONTEXT_HPP__ */