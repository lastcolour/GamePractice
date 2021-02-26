#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderNode.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Game/ETGameElem.hpp"
#include "UI/ETUIViewPort.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace {

EBoardElemMoveState getElemMoveState(EntityId elemId) {
    EBoardElemMoveState state = EBoardElemMoveState::Static;
    ET_SendEventReturn(state, elemId, &ETGameBoardElem::ET_getMoveState);
    return state;
}

EBoardElemLifeState getElemLifeState(EntityId elemId) {
    EBoardElemLifeState state = EBoardElemLifeState::Void;
    ET_SendEventReturn(state, elemId, &ETGameBoardElem::ET_getLifeState);
    return state;
}

void setElemMoveState(EntityId elemId, EBoardElemMoveState state) {
    ET_SendEvent(elemId, &ETGameBoardElem::ET_setMoveState, state);
}

void setElemLifeState(EntityId elemId, EBoardElemLifeState state) {
    ET_SendEvent(elemId, &ETGameBoardElem::ET_setLifeState, state);
}

} // namespace

GameBoardLogic::GameBoardLogic() :
    boardBox(Vec2i(0), Vec2i(0)),
    boardSize(0),
    objectSize(0.f),
    cellScale(0.9f),
    moveSpeed(1.f),
    moveAccel(4.f),
    cellSize(0),
    zBackgroundIndex(0),
    doElemMathcing(false),
    isBoardStatic(true) {
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
    doElemMathcing = true;
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
        LogWarning("[GameBoardLogic::createNewElement] Can't create board cell entity");
        return elem;
    }

    elem.boardPt = boardPt;
    setElemBoardPos(elem, elem.boardPt);
    setElemMoveState(elem.entId, EBoardElemMoveState::Static);
    setElemLifeState(elem.entId, EBoardElemLifeState::Alive);
    ET_SendEvent(elem.entId, &ETRenderNode::ET_setDrawPriority, zBackgroundIndex + 1);
    ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
    ET_SendEvent(elem.entId, &ETRenderNode::ET_show);

    return elem;
}

void GameBoardLogic::init() {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    AABB2Di visualBox(Vec2i(0), viewPort);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    visualBox.setCenter(Vec2i(static_cast<int>(tm.pt.x), static_cast<int>(tm.pt.y)));
    ET_onBoxChanged(visualBox);
    ET_onZIndexChanged(0);

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

void GameBoardLogic::updateAfterRemoves() {
    Transform tm;
    auto pos = ET_getPosFromBoardPos(Vec2i(0, boardSize.y - 1));
    float topPosY = pos.y;

    for(auto& col : columns) {
        auto it = col.begin();
        while(it != col.end()) {
            auto elem = *it;
            auto lifeState = getElemLifeState(elem.entId);
            if(lifeState != EBoardElemLifeState::Void) {
                ++it;
            } else {
                it = col.erase(it);

                ET_SendEvent(getEntityId(), &ETGameBoardElemsPool::ET_removeElem, elem.entId);
                elem = createNewElement(elem.boardPt);
                if(!elem.entId.isValid()) {
                    LogError("[GameBoardLogic::updateAfterRemoves] Can't respawn new element");
                    return;
                }

                auto& topElem = col.back();
                ET_SendEventReturn(tm, topElem.entId, &ETEntity::ET_getLocalTransform);
                if(tm.pt.y > topPosY) {
                    tm.pt.y += cellSize;
                    ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);
                } else {
                    setElemBoardPos(elem, Vec2i(elem.boardPt.x, boardSize.y));
                }

                elem.boardPt.y = -1;
                setElemMoveState(elem.entId, EBoardElemMoveState::Falling);
                setElemLifeState(elem.entId, EBoardElemLifeState::Alive);

                col.push_back(elem);
            }
        }
    }
}

void GameBoardLogic::matchElements() {
    ET_SendEvent(getEntityId(), &ETGameBoardMatcher::ET_destoryMatchedElems);
    updateAfterRemoves();
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
    for(auto& col : columns) {
        float prevYPos = -std::numeric_limits<float>::max();
        float prevVel = -1.f;
        for(int i = 0, sz = col.size(); i < sz; ++i) {
            int movePtY = i;
            auto& elem = col[i];

            auto moveState = getElemMoveState(elem.entId);
            if(moveState == EBoardElemMoveState::Static) {
                if(elem.boardPt.y != movePtY) {
                    setElemMoveState(elem.entId, EBoardElemMoveState::Falling);
                } else {
                    continue;
                }
            } else if(moveState != EBoardElemMoveState::Falling) {
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
                isBoardStatic = false;
            } else {
                elem.vel = 0.f;
                setElemMoveState(elem.entId, EBoardElemMoveState::Static);
                setElemBoardPos(elem, Vec2i(elem.boardPt.x, movePtY));
                doElemMathcing = true;
            }
        }
    }

    if(doElemMathcing) {
        doElemMathcing = false;
        matchElements();
    }
    if(!isBoardStatic) {
        isBoardStatic = ET_isAllElemStatic();
        if(isBoardStatic) {
            ET_SendEvent(&ETGameBoardEvents::ET_onAllElemsStatic);
        }
    }
}

bool GameBoardLogic::ET_isAllElemStatic() const {
    if(doElemMathcing) {
        return false;
    }
    for(auto& col : columns) {
        for(auto& elem : col) {
            auto lifeState = getElemLifeState(elem.entId);
            if(lifeState != EBoardElemLifeState::Alive) {
                return false;
            }
            auto moveState = getElemMoveState(elem.entId);
            if(moveState != EBoardElemMoveState::Static) {
                return false;
            }
        }
    }
    return true;
}

const Vec2i& GameBoardLogic::ET_getBoardSize() const {
    return boardSize;
}

const AABB2Di& GameBoardLogic::ET_getBoardBox() const {
    return boardBox;
}

void GameBoardLogic::ET_onBoxChanged(const AABB2Di& newAabb) {
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
    boardBox.setCenter(newAabb.getCenter());

    ET_SendEvent(backgroundId, &ETRenderRect::ET_setSize, Vec2(boardBoxSize.x, boardBoxSize.y));

    for(auto& col : columns) {
        for(auto& elem : col) {
            setElemBoardPos(elem, elem.boardPt);
            ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
        }
    }
}

void GameBoardLogic::ET_onZIndexChanged(int newZIndex) {
    zBackgroundIndex = newZIndex;
    ET_SendEvent(backgroundId, &ETRenderNode::ET_setDrawPriority, zBackgroundIndex);
    for(auto& col : columns) {
        for(auto& elem : col) {
            ET_SendEvent(elem.entId, &ETRenderNode::ET_setDrawPriority, zBackgroundIndex + 1);
        }
    }
}

void GameBoardLogic::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(backgroundId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
    for(auto& col : columns) {
        for(auto& elem : col) {
            ET_SendEvent(elem.entId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
        }
    }
}

void GameBoardLogic::ET_onHidden(bool flag) {
    if(flag) {
        ET_SendEvent(backgroundId, &ETRenderNode::ET_hide);
        for(auto& col : columns) {
            for(auto& elem : col) {
                ET_SendEvent(elem.entId, &ETRenderNode::ET_hide);
            }
        }
    } else {
        ET_SendEvent(backgroundId, &ETRenderNode::ET_show);
        for(auto& col : columns) {
            for(auto& elem : col) {
                ET_SendEvent(elem.entId, &ETRenderNode::ET_show);
            }
        }
    }
}

void GameBoardLogic::ET_setUIElement(EntityId rootUIElementId) {
    ETNode<ETUIElementEvents>::disconnect();
    uiBoxId = rootUIElementId;

    int zIndex = 0;
    ET_SendEventReturn(zIndex, uiBoxId, &ETUIElement::ET_getZIndex);
    ET_onZIndexChanged(zIndex);

    bool isHidden = false;
    ET_SendEventReturn(isHidden, uiBoxId, &ETUIElement::ET_isHidden);
    ET_onHidden(isHidden);

    float alpha = 1.f;
    ET_SendEventReturn(alpha, uiBoxId, &ETUIElement::ET_getAlpha);
    ET_onAlphaChanged(alpha);

    AABB2Di aabb;
    ET_SendEventReturn(aabb, uiBoxId, &ETUIElement::ET_getBox);
    ET_onBoxChanged(aabb);

    ETNode<ETUIElementEvents>::connect(uiBoxId);
}