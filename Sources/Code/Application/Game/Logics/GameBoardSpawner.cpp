#include "Game/Logics/GameBoardSpawner.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Game/ETGameBoard.hpp"
#include "UI/ETUIBox.hpp"

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

    ET_SendEvent(&ETGameBoardSpawnerEvents::ET_onStartLoading);

    ET_SendEventReturn(gameBoardId, &ETEntityManager::ET_createEntity, gameBoardName.c_str());
    if(!gameBoardId.isValid()) {
        LogWarning("[GameBoardSpawner::ET_loadPendingLevel] Can't load pending level: '%s'", gameBoardName);
        return;
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(gameBoardId, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(gameBoardId, &ETEntity::ET_setParent, getEntityId());

    AABB2D box(0.f);
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);
    ET_SendEvent(gameBoardId, &ETGameBoard::ET_resize, box);

    ET_SendEvent(gameBoardId, &ETGameBoard::ET_setUIElement, getEntityId());
}

void GameBoardSpawner::ET_onBoxChanged(const AABB2D& newAabb) {
    if(!gameBoardId.isValid()) {
        return;
    }
    ET_SendEvent(gameBoardId, &ETGameBoard::ET_resize, newAabb);
}

void GameBoardSpawner::ET_unloadLevel() {
    if(!gameBoardId.isValid()) {
        LogWarning("[GameBoardSpawner::ET_unloadLevel] No level to unload");
        return;
    }
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
    gameBoardId = InvalidEntityId;
}

void GameBoardSpawner::init() {
    ETNode<ETGameBoardSpawner>::connect(getEntityId());
    ETNode<ETUIElementEvents>::connect(getEntityId());
}

void GameBoardSpawner::deinit() {
    if(gameBoardId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
        gameBoardId = InvalidEntityId;
    }
}