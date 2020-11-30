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

namespace {

const int INVALID_BOARD_ELEM_ID = -1;

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
    std::swap(firstElem->boardPt, secondElem->boardPt);
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
    Vec2i boardPt = pt -  boardBox.bot;
    boardPt /= cellSize;
    boardPt.x /= boardSize.x;
    boardPt.y /= boardSize.y;
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
    elem.movePt = boardPt;
    setElemBoardPos(elem, elem.boardPt);
    setElemMoveState(elem.entId, EBoardElemMoveState::Static);
    setElemLifeState(elem.entId, EBoardElemLifeState::Alive);
    ET_SendEvent(elem.entId, &ETRenderNode::ET_setDrawPriority, zBackgroundIndex + 1);
    ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
    ET_SendEvent(elem.entId, &ETRenderNode::ET_show);

    return elem;
}

bool GameBoardLogic::init() {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    AABB2Di visualBox(Vec2i(0), viewPort);

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    visualBox.setCenter(Vec2i(static_cast<int>(tm.pt.x), static_cast<int>(tm.pt.y)));
    ET_onBoxChanged(visualBox);
    ET_onZIndexChanged(0);

    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            auto elem = createNewElement(Vec2i(i, j));
            if(!elem.entId.isValid()) {
                return false;
            } else {
                elements.push_back(elem);
            }
        }
    }

    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoard>::connect(getEntityId());
    return true;
}

void GameBoardLogic::deinit() {
}

const BoardElement* GameBoardLogic::getElem(const Vec2i& boardPt) const {
    for(auto& elem : elements) {
        if(elem.boardPt == boardPt) {
            return &elem;
        }
    }
    return nullptr;
}

const BoardElement* GameBoardLogic::getElem(EntityId entId) const {
    for(auto& elem : elements) {
        if(elem.entId == entId) {
            return &elem;
        }
    }
    return nullptr;
}

BoardElement* GameBoardLogic::getElem(EntityId entId) {
    for(auto& elem : elements) {
        if(elem.entId == entId) {
            return &elem;
        }
    }
    return nullptr;
}

BoardElement* GameBoardLogic::getElem(const Vec2i& boardPt) {
    for(auto& elem : elements) {
        if(elem.boardPt == boardPt) {
            return &elem;
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
    for(auto& elem : elements) {
        auto lifeState = getElemLifeState(elem.entId);
        if(lifeState == EBoardElemLifeState::Void) {
            removeElem(elem);
        }
    }
}

void GameBoardLogic::matchElements() {
    std::vector<EntityId> elemToRemove;
    ET_SendEventReturn(elemToRemove, getEntityId(), &ETGameBoardMatcher::ET_getMatchedElements);
    for(auto& elemEntId : elemToRemove) {
        auto boardElem = getElem(elemEntId);
        assert(boardElem && "Try to remove invalid board elem");
        ET_SendEvent(boardElem->entId, &ETGameBoardElem::ET_triggerDestroy);
    }
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

void GameBoardLogic::updateMovePosition(BoardElement& elem) {
    auto moveSate = getElemMoveState(elem.entId);
    auto elemCount = getAllElemsBelow(elem);

    if(moveSate == EBoardElemMoveState::Static) {
        if(elem.boardPt.y == elemCount) {
            return;
        } else {
            setElemMoveState(elem.entId, EBoardElemMoveState::Falling);
            elem.boardPt.y = -1;
        }
    } else if(moveSate != EBoardElemMoveState::Falling) {
        return;
    }

    elem.movePt.x = elem.boardPt.x;
    elem.movePt.y = elemCount;
}

void GameBoardLogic::moveElem(BoardElement& elem, float dt) {
    if(elem.boardPt.y != -1) {
        return;
    }

    isBoardStatic = false;

    Transform tm;
    ET_SendEventReturn(tm, elem.entId, &ETEntity::ET_getLocalTransform);

    elem.vel += dt * moveAccel;
    tm.pt.y -= (moveSpeed + elem.vel) * dt * cellSize;
    Vec3 desirePt = ET_getPosFromBoardPos(elem.movePt);
    if(tm.pt.y > desirePt.y) {
        ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);
    } else {
        elem.vel = 0.f;
        setElemMoveState(elem.entId, EBoardElemMoveState::Static);
        setElemBoardPos(elem, elem.movePt);
        doElemMathcing = true;
    }
}

void GameBoardLogic::ET_onGameTick(float dt) {
    for(auto& elem : elements) {
        updateMovePosition(elem);
    }
    for(auto& elem : elements) {
        moveElem(elem, dt);
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
    for(auto& elem : elements) {
        auto lifeState = getElemLifeState(elem.entId);
        if(lifeState != EBoardElemLifeState::Alive) {
            return false;
        }
        auto moveState = getElemMoveState(elem.entId);
        if(moveState != EBoardElemMoveState::Static) {
            return false;
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

    for(auto& elem : elements) {
        setElemBoardPos(elem, elem.boardPt);
        ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
    }
}

void GameBoardLogic::ET_onZIndexChanged(int newZIndex) {
    zBackgroundIndex = newZIndex;
    ET_SendEvent(backgroundId, &ETRenderNode::ET_setDrawPriority, zBackgroundIndex);
    for(auto& elem : elements) {
        ET_SendEvent(elem.entId, &ETRenderNode::ET_setDrawPriority, zBackgroundIndex + 1);
    }
}

void GameBoardLogic::ET_onAlphaChanged(float newAlpha) {
    ET_SendEvent(backgroundId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
    for(auto& elem : elements) {
        ET_SendEvent(elem.entId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
    }
}

void GameBoardLogic::ET_onHidden(bool flag) {
    if(flag) {
        for(auto& elem : elements) {
            ET_SendEvent(elem.entId, &ETRenderNode::ET_hide);
        }
    } else {
        for(auto& elem : elements) {
            ET_SendEvent(elem.entId, &ETRenderNode::ET_show);
        }
    }
}

void GameBoardLogic::ET_onDisabled(bool flag) {
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

void GameBoardLogic::removeElem(BoardElement& elem) {
    ET_SendEvent(getEntityId(), &ETGameBoardElemsPool::ET_removeElem, elem.entId);
    elem = createNewElement(elem.boardPt);
    if(!elem.entId.isValid()) {
        LogError("[GameBoardLogic::removeElem] Can't respawn new element");
        return;
    }

    auto topElem = getTopElemAbove(Vec2i(elem.boardPt.x, boardSize.y - 1));
    if(topElem) {
        Transform tm;
        ET_SendEventReturn(tm, topElem->entId, &ETEntity::ET_getLocalTransform);

        tm.pt.y += cellSize;
        ET_SendEvent(elem.entId, &ETEntity::ET_setLocalTransform, tm);

    } else {
        setElemBoardPos(elem, Vec2i(elem.boardPt.x, boardSize.y));
    }

    elem.boardPt.y = -1;

    setElemMoveState(elem.entId, EBoardElemMoveState::Falling);
    setElemLifeState(elem.entId, EBoardElemLifeState::Alive);
}

int GameBoardLogic::getAllElemsBelow(const BoardElement& topElem) {
    int count = 0;

    Transform tm;
    ET_SendEventReturn(tm, topElem.entId, &ETEntity::ET_getLocalTransform);
    float posY = tm.pt.y;

    for(auto& elem : elements) {
        if(elem.boardPt.x != topElem.boardPt.x) {
            continue;
        }
        ET_SendEventReturn(tm, elem.entId, &ETEntity::ET_getLocalTransform);
        if(tm.pt.y < posY) {
            count += 1;
        }
    }
    return count;
}

BoardElement* GameBoardLogic::getTopElemAbove(const Vec2i& boardPt) {
    BoardElement* topElem = nullptr;
    auto pos = ET_getPosFromBoardPos(boardPt);
    float topPosY = pos.y;
    for(auto& elem : elements) {
        if(elem.boardPt.x != boardPt.x) {
            continue;
        }
        Transform tm;
        ET_SendEventReturn(tm, elem.entId, &ETEntity::ET_getLocalTransform);
        if(tm.pt.y > topPosY) {
            topPosY = tm.pt.y;
            topElem = &elem;
        }
    }
    return topElem;
}