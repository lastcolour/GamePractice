#include "AsyncEntityManager.hpp"

AsyncEntityManager::AsyncEntityManager() {
}

AsyncEntityManager::~AsyncEntityManager() {
}

bool AsyncEntityManager::init() {
    ETNode<ETEntitiesUpdateTask>::connect(getEntityId());
    return true;
}

void AsyncEntityManager::deinit() {
}

void AsyncEntityManager::ET_updateEntities() {
}