#include "Game/Logics/GameBoardSpawner.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "UI/ETUIBox.hpp"

void GameBoardSpawner::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardSpawner>("GameBoardSpawner")) {
        classInfo->addResourceField("gameBoard", &GameBoardSpawner::setGameBoard);
    }
}

GameBoardSpawner::GameBoardSpawner() {
}

GameBoardSpawner::~GameBoardSpawner() {
}

void GameBoardSpawner::setGameBoard(const char* gameBoard) {
    gameBoardName = gameBoard;
}

void GameBoardSpawner::ET_onBoxResized(const AABB2Di& newAabb) {
    int zIndex = 0;
    ET_SendEventReturn(zIndex, getEntityId(), &ETUIElement::ET_getZIndex);

    ET_SendEvent(gameBoardId, &ETGameBoard::ET_setVisualParams, zIndex + 1, newAabb);
}

void GameBoardSpawner::ET_onZIndexChanged(int newZIndex) {
    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);

    ET_SendEvent(gameBoardId, &ETGameBoard::ET_setVisualParams, newZIndex + 1, box);
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

    int zIndex = 0;
    ET_SendEventReturn(zIndex, getEntityId(), &ETUIElement::ET_getZIndex);

    AABB2Di box;
    ET_SendEventReturn(box, getEntityId(), &ETUIElement::ET_getBox);

    ET_SendEvent(gameBoardId, &ETGameBoard::ET_setVisualParams, zIndex + 1, box);

    ET_SendEvent(&ETGameTimer::ET_resumeTimer);
    
    ETNode<ETUIBoxEvents>::connect(getEntityId());
    return true;
}

void GameBoardSpawner::deinit() {
    if(gameBoardId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
        gameBoardId = InvalidEntityId;
    }
}