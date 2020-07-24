#ifndef __ASYNC_ENTITY_MANAGER_HPP__
#define __ASYNC_ENTITY_MANAGER_HPP__

#include "Core/ETTasks.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"

class AsyncEntityManager : public SystemLogic,
    public ETNode<ETEntitiesUpdateTask> {
public:

    AsyncEntityManager();
    virtual ~AsyncEntityManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETEntitiesUpdateTask
    void ET_updateEntities() override;
};

#endif /* __ASYNC_ENTITY_MANAGER_HPP__ */