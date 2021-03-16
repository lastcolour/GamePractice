#ifndef __ENTITY_CONTEXT_HPP__
#define __ENTITY_CONTEXT_HPP__

#include <mutex>

class Entity;

class EntityRegistry {
public:

    using EntityContainerT = std::unordered_map<EntityId, std::unique_ptr<Entity>>;

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
    EntityContainerT entities;
};

#endif /* __ENTITY_CONTEXT_HPP__ */