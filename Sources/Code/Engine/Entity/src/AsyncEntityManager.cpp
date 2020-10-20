#include "AsyncEntityManager.hpp"
#include "Entity/EntityLoadResult.hpp"

AsyncEntityManager::AsyncEntityManager() {
}

AsyncEntityManager::~AsyncEntityManager() {
}

bool AsyncEntityManager::init() {
    ETNode<ETEntitiesUpdateTask>::connect(getEntityId());
    ETNode<ETAsyncEntityManager>::connect(getEntityId());
    return true;
}

void AsyncEntityManager::ET_createAsyncEntity(const char* entityName, std::function<void(std::shared_ptr<EntityLoadResult>)> callback) {
    CreateRequest req;
    req.name = entityName;
    req.callback = callback;
    {
        std::lock_guard<std::mutex> lock(mutex);
        pendingRequests.push_back(req);
    }
}

void AsyncEntityManager::deinit() {
}

void AsyncEntityManager::ET_updateEntities() {
    std::vector<CreateRequest> requests;
    {
        std::lock_guard<std::mutex> lock(mutex);
        requests = std::move(pendingRequests);
    }
    for(auto& req : requests) {
        EntityId entId;
        ET_SendEventReturn(entId, &ETEntityManager::ET_createUnfinishedEntity, req.name.c_str());
        std::shared_ptr<EntityLoadResult> result(new EntityLoadResult(entId));
        req.callback(std::move(result));
    }
}