#ifndef __ENTITY_LOAD_RESULT_HPP__
#define __ENTITY_LOAD_RESULT_HPP__

#include "Entity/ETEntityManager.hpp"

class EntityLoadResult {
public:

    EntityLoadResult();
    ~EntityLoadResult();

    EntityId finishLoad();
    void addEntityToFinish(EntityId finishEntId);
    void setResultEntity(EntityId resultEntId);

private:

    EntityLoadResult(const EntityLoadResult&) = delete;
    EntityLoadResult& operator=(EntityLoadResult&) = delete;

private:

    EntityId entityId;
    std::vector<EntityId> waitFinishEntities;
};

#endif /* __ENTITY_LOAD_RESULT_HPP__ */