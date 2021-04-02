#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Render/ETRenderNode.hpp"
#include "UI/ETUIViewPort.hpp"

#include <cassert>

GameBoardLogic::GameBoardLogic() :
    boardBox(0),
    boardSize(0),
    objectSize(0.f),
    cellScale(0.9f),
    moveSpeed(1.f),
    moveAccel(4.f),
    cellSize(0),
    isElemMatchRequested(false),
    isBoardStatic(true),
    isElemMatchingBlocked(false) {
}

GameBoardLogic::~GameBoardLogic() {
}

void GameBoardLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardLogic>("GameBoard")) {
        classInfo->addField("fallSpeed", &GameBoardLogic::moveSpeed);
        classInfo->addField("fallAcceleration", &GameBoardLogic::moveAccel);
        classInfo->addField("size", &GameBoardLogic::boardSize);
        classInfo->addField("cellScale", &GameBoardLogic::cellScale);
        classInfo->addField("backgroundId", &GameBoardLogic::backgroundId);
    }
}

void GameBoardLogic::ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) {
    auto firstElem = getElem(firstId);
    auto secondElem = getElem(secondId);
    if(!firstElem || !secondElem) {
        assert(false && "Try switch invalid board eleemts");
        return;
    }
    std::swap(firstElem->entId, secondElem->entId);
    setElemBoardPos(*firstElem, firstElem->boardPt);
    setElemBoardPos(*secondElem, secondElem->boardPt);
}

void GameBoardLogic::ET_matchElements() {
    isElemMatchRequested = true;
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

BoardElement GameBoardLogic::createNewElement(const Vec2i& boardPt) const {
    BoardElement elem;
    ET_SendEventReturn(elem.entId, getEntityId(), &ETGameBoardElemsPool::ET_spawnElem);
    if(!elem.entId.isValid()) {
        LogWarning("[GameBoardLogic::createNewElement] Can't spawn new board element");
        return elem;
    }

    setElemBoardPos(elem, boardPt);
    GameUtils::SetElemState(elem.entId, EBoardElemState::Static);
    ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
    ET_SendEvent(elem.entId, &ETRenderNode::ET_show);

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

    uiProxies.addItem(backgroundId, GameUtils::BACKGROUND_Z_INDEX);
    for(int i = 0; i < boardSize.x; ++i) {
        std::vector<BoardElement> column;
        for(int j = 0; j < boardSize.y; ++j) {
            auto elem = createNewElement(Vec2i(i, j));
            if(!elem.entId.isValid()) {
                return;
            } else {
                uiProxies.addItem(elem.entId, GameUtils::ELEMENT_Z_INDEX);
                column.push_back(elem);
            }
        }
        columns.push_back(std::move(column));
    }

    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoard>::connect(getEntityId());
}

void GameBoardLogic::deinit() {
}

const BoardElement* GameBoardLogic::getElem(const Vec2i& boardPt) const {
    if(boardPt.x >= columns.size()) {
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
    if(boardPt.x >= columns.size()) {
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
    Transform tm;
    auto pos = ET_getPosFromBoardPos(Vec2i(0, boardSize.y - 1));
    float topPosY = pos.y;

    for(auto& col : columns) {
        auto it = col.begin();
        while(it != col.end()) {
            auto elem = *it;
            auto elemState = GameUtils::GetElemState(elem.entId);
            if(elemState != EBoardElemState::Destroyed) {
                ++it;
            } else {
                uiProxies.removeItem(it->entId);
                it = col.erase(it);

                ET_SendEvent(getEntityId(), &ETGameBoardElemsPool::ET_removeElem, elem.entId);
                elem = createNewElement(elem.boardPt);
                if(!elem.entId.isValid()) {
                    LogError("[GameBoardLogic::updateAfterRemoves] Can't respawn new element");
                    return;
                }
                uiProxies.addItem(elem.entId, GameUtils::ELEMENT_Z_INDEX);

                bool hasTopElem = false;
                if(!col.empty()) {
                    auto& topElem = col.back();
                    ET_SendEventReturn(tm, topElem.entId, &ETEntity::ET_getLocalTransform);
                    if(tm.pt.y > topPosY) {
                        hasTopElem = true;
                        tm.pt.y += cellSize;
                        ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);
                    }
                }

                if(!hasTopElem) {
                    setElemBoardPos(elem, Vec2i(elem.boardPt.x, boardSize.y));
                }

                elem.boardPt.y = -1;
                GameUtils::SetElemState(elem.entId, EBoardElemState::Falling);
                col.push_back(elem);
            }
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
    if(!isBoardStatic) {
        respawnDestroyedElems();
    }

    int nonStaticElemCount = 0;
    for(auto& col : columns) {
        float prevYPos = -std::numeric_limits<float>::max();
        float prevVel = -1.f;
        for(int i = 0, sz = static_cast<int>(col.size()); i < sz; ++i) {
            int movePtY = i;
            auto& elem = col[i];

            auto elemState = GameUtils::GetElemState(elem.entId);
            if(elemState == EBoardElemState::Static) {
                if(elem.boardPt.y != movePtY) {
                    elemState = EBoardElemState::Falling;
                    GameUtils::SetElemState(elem.entId, elemState);
                }
            }
            if(elemState != EBoardElemState::Static) {
                ++nonStaticElemCount;
            }
            if(elemState != EBoardElemState::Falling) {
                continue;
            }

            Transform tm;
            ET_SendEventReturn(tm, elem.entId, &ETEntity::ET_getLocalTransform);

            elem.vel += dt * moveAccel;
            tm.pt.y -= (moveSpeed + elem.vel) * dt * cellSize;

            Vec3 desirePt = ET_getPosFromBoardPos(Vec2i(elem.boardPt.x, movePtY));
            bool reachDestination = tm.pt.y <= desirePt.y;

            if(tm.pt.y < (prevYPos + cellSize)) {
                tm.pt.y = prevYPos + cellSize;
                elem.vel = prevVel * 0.9f;
            }

            prevYPos = tm.pt.y;
            prevVel = elem.vel;

            if(!reachDestination) {
                ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);
            } else {
                elem.vel = 0.f;
                setElemBoardPos(elem, Vec2i(elem.boardPt.x, movePtY));
                ET_SendEvent(elem.entId, &ETGameBoardElem::ET_triggerLand);
                if(GameUtils::GetElemState(elem.entId) == EBoardElemState::Static) {
                    --nonStaticElemCount;
                    isElemMatchRequested = true;
                }
            }
        }
    }

    isBoardStatic = nonStaticElemCount == 0;
    if(isBoardStatic) {
        if(isElemMatchRequested && !isElemMatchingBlocked) {
            isElemMatchRequested = false;
            isBoardStatic = false;
            ET_SendEvent(&ETGameBoardMatcher::ET_destoryMatchedElems);
        }
    }
    if(isBoardStatic) {
        ET_SendEvent(&ETGameBoardEvents::ET_onAllElemsStatic);
    }
}

bool GameBoardLogic::ET_isAllElemStatic() const {
    if(isElemMatchRequested) {
        return false;
    }
    return isBoardStatic;
}

void GameBoardLogic::ET_updateBoardMatchState(BoardMatchState& boardMatchState) const {
    boardMatchState.setSize(boardSize);
    for(auto& col : columns) {
        for(auto& elem : col) {
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
    isElemMatchingBlocked = flag;
}

void GameBoardLogic::ET_resize(const AABB2D& newAabb) {
    auto aabbSize = newAabb.getSize();

    assert(aabbSize.x > 0 && "Invalid size of game board box");
    assert(aabbSize.y > 0 && "Invalid size of game board box");

    float cellSizeX = aabbSize.x / static_cast<float>(boardSize.x);
    float cellSizeY = aabbSize.y / static_cast<float>(boardSize.y);
    cellSize = static_cast<int>(floor(std::min(cellSizeX, cellSizeY)));
    objectSize = Vec2(floor(cellSize * cellScale));
    Vec2i boardBoxSize = Vec2i(cellSize * boardSize.x, cellSize * boardSize.y);

    boardBox.bot = Vec2i(0);
    boardBox.top = boardBoxSize;
    Vec2 center = newAabb.getCenter();
    boardBox.setCenter(static_cast<int>(center.x), static_cast<int>(center.y));

    ET_SendEvent(backgroundId, &ETRenderRect::ET_setSize, Vec2(
        static_cast<float>(boardBoxSize.x), static_cast<float>(boardBoxSize.y)));

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