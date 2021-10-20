#include "Game/Logics/GameBoardSpawner.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Game/ETGameBoard.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderNode.hpp"

namespace {

const int GAME_BOARD_Z_INDEX_OFFSET = 10;

} // namespace

void GameBoardSpawner::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardSpawner>("GameBoardSpawner")) {
        (void)classInfo;
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
    if(!ET_IsExistNode<ETUIElement>(getEntityId())) {
        LogError("[GameBoardSpawner::ET_loadPendingLevel] Can't find 'ETUIElement' on game board spawner: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
    }
    ET_SendEventReturn(box, getEntityId(), &ETUIElementGeom::ET_getBox);
    ET_SendEvent(gameBoardId, &ETGameBoard::ET_resize, box);

    ET_SendEventReturn(gameBoardRootRenderId, gameBoardId, &ETGameBoard::ET_getRootRenderId);
    if(!gameBoardRootRenderId.isValid()) {
        LogError("[GameBoardSpawner::ET_loadPendingLevel] Can't find root render object on game board '%s'",
            EntityUtils::GetEntityName(gameBoardId));
    }

    ET_SendEvent(gameBoardRootRenderId, &ETRenderNode::ET_hide);
    ET_SendEvent(&ETGameBoard::ET_spawnElems);
}

void GameBoardSpawner::ET_onZIndexChanged(int newZIndex) {
    ET_SendEvent(gameBoardRootRenderId, &ETRenderNode::ET_setZIndex, newZIndex + GAME_BOARD_Z_INDEX_OFFSET);
}

void GameBoardSpawner::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(gameBoardRootRenderId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void GameBoardSpawner::ET_onHidden(bool flag) {
    if(flag) {
        ET_SendEvent(gameBoardRootRenderId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(gameBoardRootRenderId, &ETRenderNode::ET_show);
    }
}

void GameBoardSpawner::ET_onBoxChanged(const AABB2D& newAabb) {
    ET_SendEvent(gameBoardId, &ETGameBoard::ET_resize, newAabb);
}

void GameBoardSpawner::ET_unloadLevel() {
    if(!gameBoardId.isValid()) {
        LogWarning("[GameBoardSpawner::ET_unloadLevel] No level to unload");
        return;
    }
    ET_SendEvent(&ETGameBoardSpawnerEvents::ET_onStartDestroying);
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, gameBoardId);
    gameBoardId = InvalidEntityId;
}

EntityId GameBoardSpawner::ET_getGameBoard() const {
    return gameBoardId;
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