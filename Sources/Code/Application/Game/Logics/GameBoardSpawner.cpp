#include "Game/Logics/GameBoardSpawner.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "UI/ETUIBox.hpp"

void GameBoardSpawner::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardSpawner>("GameBoardSpawner")) {
        classInfo->addResourceField("gameBoard", ResourceType::Entity, &GameBoardSpawner::gameBoardName);
    }
}

GameBoardSpawner::GameBoardSpawner() {
}

GameBoardSpawner::~GameBoardSpawner() {
}

bool GameBoardSpawner::init() {
    if(gameBoardName.empty()) {
        return true;
    }

    ET_SendEventReturn(gameBoardId, &ETEntityManager::ET_createEntity, gameBoardName.c_str());
    if(!gameBoardId.isValid()) {
        return false;
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(gameBoardId, &ETEntity::ET_setTransform, tm);
    ET_SendEvent(gameBoardId, &ETEntity::ET_setParent, getEntityId());

    ET_SendEvent(gameBoardId, &ETGameBoard::ET_setUIElement, getEntityId());

    return true;
}

void GameBoardSpawner::deinit() {
    if(gameBoardId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
        gameBoardId = InvalidEntityId;
    }
}