#ifndef __ENTITY_CONTEXT_HPP__
#define __ENTITY_CONTEXT_HPP__

#include "Core/ObjectPool.hpp"
#include "Entity.hpp"

class EntityRegistry {
public:

    EntityRegistry();
    ~EntityRegistry();

    Entity* createEntity(const char* name);
    Entity* findEntity(EntityId entityId);
    void removeEntity(Entity* entity);
    void removeEntity(EntityId entity);
    void removeAllEntities();

    size_t getEntitiesCount() const;

private:

    mutable std::recursive_mutex mutex;
    Memory::ObjectPool<Entity> pool;
    std::unordered_map<EntityId, Entity*> searchTable;
};

#endif /* __ENTITY_CONTEXT_HPP__ */