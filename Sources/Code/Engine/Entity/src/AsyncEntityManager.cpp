#include "AsyncEntityManager.hpp"

AsyncEntityManager::AsyncEntityManager() {
}

AsyncEntityManager::~AsyncEntityManager() {
}

bool AsyncEntityManager::init() {
    ETNode<ETEntitiesUpdateTask>::connect(getEntityId());
    ETNode<ETAsyncEntityManager>::connect(getEntityId());
    return true;
}

void AsyncEntityManager::ET_createAsyncEntity(const char* entityName, std::function<void(EntityId)> callback) {
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
        ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, req.name.c_str());
        req.callback(entId);
    }
}