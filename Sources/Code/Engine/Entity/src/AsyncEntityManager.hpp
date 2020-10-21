#ifndef __ASYNC_ENTITY_MANAGER_HPP__
#define __ASYNC_ENTITY_MANAGER_HPP__

#include "Core/ETTasks.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"
#include "Entity/ETEntityManger.hpp"

#include <mutex>
#include <vector>

class AsyncEntityManager : public SystemLogic,
    public ETNode<ETEntitiesUpdateTask>,
    public ETNode<ETAsyncEntityManager> {
public:

    AsyncEntityManager();
    virtual ~AsyncEntityManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAsyncEntityManager
    void ET_createAsyncEntity(const char* entityName, std::function<void(std::shared_ptr<EntityLoadResult>)> callback) override;
    bool ET_isInsideAsyncLoad() const override;
    void ET_addEntityToFinishLater(EntityId entityId) override;

    // ETEntitiesUpdateTask
    void ET_updateEntities() override;

private:

    struct CreateRequest {
        std::string name;
        std::function<void(std::shared_ptr<EntityLoadResult>)> callback;
    };

private:

    std::mutex mutex;
    std::thread::id asyncLoadThreadId;
    std::vector<CreateRequest> pendingRequests;
    EntityLoadResult* activeLoadResult;
};

#endif /* __ASYNC_ENTITY_MANAGER_HPP__ */