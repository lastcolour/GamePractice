#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameElemsPool.hpp"
#include "Render/ETRenderNode.hpp"
#include "UI/ETUIViewPort.hpp"

#include <cassert>

namespace {

const Vec2i MAX_BOARD_SIZE = Vec2i(10, 10);

bool isElemMovingState(EBoardElemState elemState) {
    return elemState == EBoardElemState::Falling || elemState == EBoardElemState::Landing;
}

} // namespace

void GameBoardLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardLogic>("GameBoard")) {
        classInfo->addField("fallSpeed", &GameBoardLogic::moveSpeed);
        classInfo->addField("fallAcceleration", &GameBoardLogic::moveAccel);
        classInfo->addField("size", &GameBoardLogic::boardSize);
        classInfo->addField("cellScale", &GameBoardLogic::cellScale);
        classInfo->addField("backgroundId", &GameBoardLogic::backgroundId);
    }
}

GameBoardLogic::GameBoardLogic() :
    boardBox(0),
    boardSize(0),
    objectSize(0.f),
    cellScale(0.9f),
    moveSpeed(1.f),
    moveAccel(4.f),
    cellSize(0) {
}

GameBoardLogic::~GameBoardLogic() {
}

void GameBoardLogic::ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) {
    auto firstElem = getElem(firstId);
    auto secondElem = getElem(secondId);
    if(!firstElem || !secondElem) {
        assert(false && "Try switch invalid board elems");
        return;
    }

    ET_SendEvent(&ETGameBoardMatcher::ET_setLastSwappedElem, firstId);

    std::swap(firstElem->entId, secondElem->entId);
    setElemBoardPos(*firstElem, firstElem->boardPt);
    setElemBoardPos(*secondElem, secondElem->boardPt);

    gameBoardFSM.getState().isMatchRequested = true;
}

void GameBoardLogic::ET_replaceElemToSpecial(EntityId targetId, EBoardElemType elemType) {
    auto elem = getElem(targetId);
    if(!elem) {
        assert(false && "Try replace invalid board elem");
        return;
    }

    ET_SendEvent(&ETGameBoardElemsPool::ET_removeElem, elem->entId);

    elem->entId = InvalidEntityId;

    ET_SendEventReturn(elem->entId, &ETGameBoardElemsPool::ET_spawnElem,
        elemType);

    if(!elem->entId.isValid()) {
        LogError("[GameBoardLogic::ET_replaceElemToSpecial] Can't create special elem");
        return;
    }

    setupElem(*elem, elem->boardPt);
}

EntityId GameBoardLogic::ET_getElemByPos(const Vec2i& pt) const {
    if(!boardBox.isInside(pt)) {
        return InvalidEntityId;
    }
    Vec2i boardPt(pt.x - boardBox.bot.x, pt.y - boardBox.bot.y);
    boardPt /= cellSize;
    return ET_getElemByBoardPos(boardPt);
}

EntityId GameBoardLogic::ET_getElemByBoardPos(const Vec2i& boardPt) const {
    if(auto elem = getElem(boardPt)) {
        return elem->entId;
    }
    return InvalidEntityId;
}

Vec2i GameBoardLogic::ET_getElemBoardPos(EntityId elemEntId) const {
    if(auto elem = getElem(elemEntId)) {
        return elem->boardPt;
    }
    return Vec2i(-1);
}

int GameBoardLogic::ET_getCellSize() const {
    return cellSize;
}

BoardElement GameBoardLogic::createNewElement(const Vec2i& boardPt) {
    BoardElement elem;
    ET_SendEventReturn(elem.entId, &ETGameBoardElemsPool::ET_spawnBaseRandomElem);
    if(!elem.entId.isValid()) {
        LogWarning("[GameBoardLogic::createNewElement] Can't spawn new board element");
        return elem;
    }
    setupElem(elem, boardPt);
    return elem;
}

void GameBoardLogic::init() {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    AABB2D visualBox(Vec2(0.f), Vec2(static_cast<float>(viewPort.x), static_cast<float>(viewPort.y)));

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    visualBox.setCenter(tm.pt.x, tm.pt.y);
    ET_resize(visualBox);

    ET_SendEvent(backgroundId, &ETRenderNode::ET_show);
    uiProxies.addItem(backgroundId, GameUtils::BOARD_BACKGROUND_Z_OFFSET);

    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoard>::connect(getEntityId());
}

void GameBoardLogic::deinit() {
}

void GameBoardLogic::ET_spawnElems() {
    if(!columns.empty()) {
        LogError("[GameBoardLogic::ET_spawnElems] Elems already spawned");
        return;
    }
    for(int i = 0; i < boardSize.x; ++i) {
        std::vector<BoardElement> column;
        for(int j = 0; j < boardSize.y; ++j) {
            auto elem = createNewElement(Vec2i(i, j));
            if(!elem.entId.isValid()) {
                return;
            } else {
                column.push_back(elem);
            }
        }
        columns.push_back(std::move(column));
    }
}

const BoardElement* GameBoardLogic::getElem(const Vec2i& boardPt) const {
    if(boardPt.x >= static_cast<int>(columns.size())) {
        return nullptr;
    }
    auto& col = columns[boardPt.x];
    for(auto& elem : col) {
        if(elem.boardPt.y == boardPt.y) {
            return &elem;
        }
    }
    return nullptr;
}

BoardElement* GameBoardLogic::getElem(const Vec2i& boardPt) {
    if(boardPt.x >= static_cast<int>(columns.size())) {
        return nullptr;
    }
    auto& col = columns[boardPt.x];
    for(auto& elem : col) {
        if(elem.boardPt.y == boardPt.y) {
            return &elem;
        }
    }
    return nullptr;
}

const BoardElement* GameBoardLogic::getElem(EntityId entId) const {
    for(auto& col : columns) {
        for(auto& elem : col) {
            if(elem.entId == entId) {
                return &elem;
            }
        }
    }
    return nullptr;
}

BoardElement* GameBoardLogic::getElem(EntityId entId) {
    for(auto& col : columns) {
        for(auto& elem : col) {
            if(elem.entId == entId) {
                return &elem;
            }
        }
    }
    return nullptr;
}

Vec3 GameBoardLogic::ET_getPosFromBoardPos(const Vec2i& boardPt) const {
    Vec2 midPt(boardSize.x / 2.f, boardSize.y / 2.f);
    Vec2 resPt;
    resPt.x = (boardPt.x - midPt.x + 0.5f) * cellSize;
    resPt.y = (boardPt.y - midPt.y + 0.5f) * cellSize;
    return Vec3(resPt, 0.f);
}

void GameBoardLogic::setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const {
    elem.boardPt = boardPt;

    Transform tm;
    tm.pt = ET_getPosFromBoardPos(boardPt);
    ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);
}

void GameBoardLogic::respawnDestroyedElems() {
    gameBoardFSM.getState().isRespawnRequested = false;

    for(size_t i = 0u, sz = columns.size(); i < sz; ++i) {
        auto& col = columns[i];
        auto it = col.begin();
        while(it != col.end()) {
            auto elem = *it;
            auto elemState = GameUtils::GetElemState(elem.entId);
            if(elemState == EBoardElemState::Destroyed) {
                ET_SendEvent(it->entId, &ETGameBoardRenderElem::ET_deinitRender, uiProxies);
                if(elem.entId.isValid()) {
                    ET_SendEvent(&ETGameBoardElemsPool::ET_removeElem, elem.entId);
                }
                it = col.erase(it);
                gameBoardFSM.getState().hasMovingElems = true;
            } else {
                if(elemState != EBoardElemState::Static) {
                    gameBoardFSM.getState().isRespawnRequested = true;
                }
                ++it;
            }
        }
        auto newCount = boardSize.y -  col.size();
        for(size_t j = 0u; j < newCount; ++j) {
            auto elemBoardPt = Vec2i(static_cast<int>(i),
                boardSize.y + static_cast<int>(j));
            auto elem = createNewElement(elemBoardPt);
            if(!elem.entId.isValid()) {
                return;
            }
            elem.boardPt.y = -1;
            auto elemState = EBoardElemState::Falling;
            GameUtils::SetElemState(elem.entId, elemState);
            col.push_back(elem);
        }
    }
}

Vec2i GameBoardLogic::getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const {
    Vec2i resPt;
    resPt.x = boardPt.x;
    Vec3 pos = pt - ET_getPosFromBoardPos(boardPt);
    float yShift = pos.y;
    yShift = yShift / cellSize;
    int shift = static_cast<int>(yShift);
    if(std::abs(yShift - static_cast<float>(shift)) > 0.5f) {
        shift > 0 ? ++shift : --shift;
    }
    resPt.y = std::max(0, boardPt.y + shift);
    return resPt;
}

void GameBoardLogic::ET_onGameTick(float dt) {
    auto& state = gameBoardFSM.getState();
    ET_SendEventReturn(state.hasMergingElems, &ETGameBoardElemMergeManager::ET_hasMergeTasks);
    ET_SendEventReturn(state.hasTriggeringElems, &ETGameBoardElemTriggerManager::ET_hasTriggerTasks);
    ET_SendEventReturn(state.hasSwitchingElems, &ETGameBoardInteractionLogic::ET_hasActiveSwitching);

    auto pass = EGameBoardUpdatePass::Static;
    auto hasUpdatePass = gameBoardFSM.queryPass(pass);
    while(hasUpdatePass) {
        switch(pass) {
            case EGameBoardUpdatePass::Move: {
                gameBoardFSM.getState().isMatchRequested = true;
                processMovingElems(dt);
                break;
            }
            case EGameBoardUpdatePass::Merge: {
                gameBoardFSM.getState().isMatchRequested = true;
                gameBoardFSM.getState().isRespawnRequested = true;
                ET_SendEvent(&ETGameBoardElemMergeManager::ET_updateMergeTasks, dt);
                break;
            }
            case EGameBoardUpdatePass::Trigger: {
                gameBoardFSM.getState().isMatchRequested = true;
                gameBoardFSM.getState().isRespawnRequested = true;
                ET_SendEvent(&ETGameBoardElemTriggerManager::ET_updateTriggerTasks, dt);
                break;
            }
            case EGameBoardUpdatePass::Match: {
                gameBoardFSM.getState().isMatchRequested = false;
                bool hasAnyMatches = false;
                ET_SendEventReturn(hasAnyMatches, &ETGameBoardMatcher::ET_matchElements);
                gameBoardFSM.getState().isRespawnRequested = hasAnyMatches;
                break;
            }
            case EGameBoardUpdatePass::Respawn: {
                respawnDestroyedElems();
                break;
            }
            case EGameBoardUpdatePass::Switch: {
                break;
            }
            case EGameBoardUpdatePass::Static: {
                ET_SendEvent(&ETGameBoardEvents::ET_onAllElemsStatic);
                break;
            }
            default: {
                assert(false && "Invalid update pass");
            }
        }
        hasUpdatePass = gameBoardFSM.querySubPass(pass);
    }
}

bool GameBoardLogic::ET_isAllElemStatic() const {
    return !gameBoardFSM.canQueryPass();
}

void GameBoardLogic::ET_readBoardMatchState(BoardMatchState& boardMatchState) const {
    boardMatchState.setSize(boardSize);
    for(auto& col : columns) {
        for(auto& elem : col) {
            if(GameUtils::HasTriggerLogic(elem.entId)) {
                continue;
            }
            auto elemState = GameUtils::GetElemState(elem.entId);
            if(elemState == EBoardElemState::Static) {
                boardMatchState.setElem(elem.boardPt, elem.entId);
            }
        }
    }
}

const Vec2i& GameBoardLogic::ET_getBoardSize() const {
    return boardSize;
}

const AABB2Di& GameBoardLogic::ET_getBoardBox() const {
    return boardBox;
}

void GameBoardLogic::ET_setBlockElemMatching(bool flag) {
    gameBoardFSM.getState().isMatchBlocked = flag;
}

void GameBoardLogic::ET_resize(const AABB2D& newAabb) {
    auto aabbSize = newAabb.getSize();

    assert(aabbSize > Vec2(0.f) && "Invalid size of game board box");

    float cellSizeX = aabbSize.x / static_cast<float>(MAX_BOARD_SIZE.x);
    float cellSizeY = aabbSize.y / static_cast<float>(MAX_BOARD_SIZE.y);
    cellSize = static_cast<int>(floor(std::min(cellSizeX, cellSizeY)));
    objectSize = Vec2(floor(cellSize * cellScale));
    Vec2i boardBoxSize = Vec2i(cellSize * boardSize.x, cellSize * boardSize.y);

    boardBox.bot = Vec2i(0);
    boardBox.top = boardBoxSize;
    Vec2 center = newAabb.getCenter();
    boardBox.setCenter(static_cast<int>(center.x), static_cast<int>(center.y));

    ET_SendEvent(backgroundId, &ETRenderRect::ET_setSize, Vec2(
        static_cast<float>(boardBoxSize.x), static_cast<float>(boardBoxSize.y)));

    if(!columns.empty()) {
        LogWarning("[GameBoardLogic::ET_resize] Resize of non-empty board may-cause elements jumps!");
    }

    for(auto& col : columns) {
        for(auto& elem : col) {
            setElemBoardPos(elem, elem.boardPt);
            ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
        }
    }
}

void GameBoardLogic::ET_setUIElement(EntityId rootUIElementId) {
    uiProxies.setUIParent(rootUIElementId);
}

void GameBoardLogic::moveElem(BoardElement& elem, BoardElement* prevElem, const Vec2i& boardPt, float dt) {
    Transform tm;
    ET_SendEventReturn(tm, elem.entId, &ETEntity::ET_getLocalTransform);

    elem.vel += dt * moveAccel;
    tm.pt.y -= (moveSpeed + elem.vel) * dt * cellSize;

    bool blocked = false;

    if(prevElem) {
        auto elemState = GameUtils::GetElemState(prevElem->entId);
        if(elemState != EBoardElemState::Static && elemState != EBoardElemState::Landing) {
            Transform prevTm;
            ET_SendEventReturn(prevTm, prevElem->entId, &ETEntity::ET_getLocalTransform);

            if(tm.pt.y < (prevTm.pt.y + cellSize)) {
                blocked = true;
                tm.pt.y = prevTm.pt.y + cellSize;
                elem.vel = prevElem->vel * 0.9f;
            }
        }
    }

    Vec3 desiredPt = ET_getPosFromBoardPos(boardPt);
    if(tm.pt.y > desiredPt.y) {
        ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);
    } else if(!blocked) {
        elem.vel = 0.f;
        setElemBoardPos(elem, boardPt);
        ET_SendEvent(elem.entId, &ETGameBoardElem::ET_triggerLand);
    }
}

void GameBoardLogic::processMovingElems(float dt) {
    gameBoardFSM.getState().hasMovingElems = false;
    for(int i = 0; i < boardSize.x; ++i) {
        BoardElement* prevElem = nullptr;
        for(int j = 0; j < boardSize.y; ++j) {
            Vec2i pt(i, j);
            auto& elem = columns[i][j];
            auto elemState = GameUtils::GetElemState(elem.entId);
            if(elemState == EBoardElemState::Static) {
                if(elem.boardPt != pt) {
                    elem.boardPt = Vec2i(i, -1);
                    elemState = EBoardElemState::Falling;
                    GameUtils::SetElemState(elem.entId, elemState);
                }
            }
            if(elemState == EBoardElemState::Falling) {
                moveElem(elem, prevElem, pt, dt);
                gameBoardFSM.getState().isMatchRequested = true;
            }
            elemState = GameUtils::GetElemState(elem.entId);
            if(isElemMovingState(elemState)) {
                gameBoardFSM.getState().hasMovingElems = true;
            }
            prevElem = &elem;
        }
    }
}

void GameBoardLogic::setupElem(BoardElement& elem, const Vec2i& boardPt) {
    ET_SendEvent(getEntityId(), &ETEntity::ET_addChild, elem.entId);
    setElemBoardPos(elem, boardPt);
    GameUtils::SetElemState(elem.entId, EBoardElemState::Static);
    ET_SendEvent(elem.entId, &ETGameBoardRenderElem::ET_initRender, uiProxies, objectSize);
}

std::vector<EntityId> GameBoardLogic::ET_getAllElemsOfType(EBoardElemType queryElemType) const {
    std::vector<EntityId> res;
    for(auto& col : columns) {
        for(auto& elem : col) {
            auto elemState = GameUtils::GetElemState(elem.entId);
            if(elemState != EBoardElemState::Static) {
                continue;
            }
            auto elemType = GameUtils::GetElemType(elem.entId);
            if(elemType != queryElemType) {
                continue;
            }
            res.push_back(elem.entId);
        }
    }
    return res;
}