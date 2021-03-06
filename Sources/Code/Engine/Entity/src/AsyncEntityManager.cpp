#include "AsyncEntityManager.hpp"
#include "Entity/EntityLoadResult.hpp"
#include "Platform/ETSurface.hpp"

#include <cassert>

namespace {

bool canCreateEntities() {
    bool canRender = true;
    ET_SendEventReturn(canRender, &ETSurface::ET_canRender);
    return canRender;
}

} // namespace

AsyncEntityManager::AsyncEntityManager() :
    activeLoadResult(nullptr) {
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

bool AsyncEntityManager::ET_isInsideAsyncLoad() const {
    return std::this_thread::get_id() == asyncLoadThreadId;
}

void AsyncEntityManager::ET_addEntityToFinishLater(EntityId loadEntId) {
    assert(activeLoadResult && "Invalid active load result");
    activeLoadResult->addEntityToFinish(loadEntId);
}

void AsyncEntityManager::ET_updateEntities() {
    if(!canCreateEntities()) {
        return;
    }
    std::vector<CreateRequest> requests;
    {
        std::lock_guard<std::mutex> lock(mutex);
        requests = std::move(pendingRequests);
    }
    asyncLoadThreadId = std::this_thread::get_id();
    for(auto& req : requests) {
        std::shared_ptr<EntityLoadResult> result(new EntityLoadResult());
        activeLoadResult = result.get();

        EntityId entId;
        ET_SendEventReturn(entId, &ETEntityManager::ET_createUnfinishedEntity, req.name.c_str());
        result->setResultEntity(entId);
        activeLoadResult = nullptr;

        req.callback(result);
    }
    asyncLoadThreadId = std::thread::id();
}