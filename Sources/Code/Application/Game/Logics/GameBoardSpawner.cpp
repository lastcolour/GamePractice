#include "Game/Logics/GameBoardSpawner.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/ETGameBoard.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETLogger.hpp"

void GameBoardSpawner::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardSpawner>("GameBoardSpawner")) {
    }
}

GameBoardSpawner::GameBoardSpawner() {
}

GameBoardSpawner::~GameBoardSpawner() {
}

void GameBoardSpawner::ET_setPendingLevel(const std::string& levelName) {
    gameBoardName = levelName;
}

const char* GameBoardSpawner::ET_getLevelName() const {
    return gameBoardName.c_str();
}

void GameBoardSpawner::ET_loadPendingLevel() {
    if(gameBoardId.isValid()) {
        LogWarning("[GameBoardSpawner::ET_loadPendingLevel] Can't load level when another already loaded: '%s'",
            EntityUtils::GetEntityName(gameBoardId));
        return;
    }
    gameBoardName.clear();
    ET_PollAllEvents<ETGameBoardSpawner>();
    if(gameBoardName.empty()) {
        LogWarning("[GameBoardSpawner::ET_loadPendingLevel] Pending level name is empty");
        return;
    }

    ET_SendEventReturn(gameBoardId, &ETEntityManager::ET_createEntity, gameBoardName.c_str());
    if(!gameBoardId.isValid()) {
        LogWarning("[GameBoardSpawner::ET_loadPendingLevel] Can't load pending level: '%s'", gameBoardName);
        return;
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(gameBoardId, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(gameBoardId, &ETEntity::ET_setParent, getEntityId());

    ET_SendEvent(gameBoardId, &ETGameBoard::ET_setUIElement, getEntityId());
}

void GameBoardSpawner::ET_unloadLevel() {
    if(!gameBoardId.isValid()) {
        LogWarning("[GameBoardSpawner::ET_unloadLevel] No level to unload");
        return;
    }
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
    gameBoardId = InvalidEntityId;
}

bool GameBoardSpawner::init() {
    ETNode<ETGameBoardSpawner>::connect(getEntityId());
    return true;
}

void GameBoardSpawner::deinit() {
    if(gameBoardId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
        gameBoardId = InvalidEntityId;
    }
}