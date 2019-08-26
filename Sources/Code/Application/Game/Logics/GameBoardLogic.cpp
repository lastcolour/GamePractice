#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "UI/UIETInterfaces.hpp"
#include "UI/UIStyle.hpp"

#include <algorithm>
#include <cassert>

namespace {

const int INVALID_BOARD_ELEM_ID = -1;

} // namespace

GameBoardLogic::GameBoardLogic() :
    boardSize(0),
    cellScale(1.f),
    activeTouchedElemId(INVALID_BOARD_ELEM_ID),
    doUpdate(false) {
}

GameBoardLogic::~GameBoardLogic() {
}

bool GameBoardLogic::serialize(const JSONNode& node) {
    node.read("fallSpeed", moveSpeed);
    if(moveSpeed <= 0.f) {
        LogWarning("[GameBoard::serialize] Invalid move speed: %f", moveSpeed);
        return false;
    }
    if(auto sizeNode = node.object("size")) {
        sizeNode.read("w", boardSize.x);
        if(boardSize.x <= 0) {
            LogWarning("[GameBoard::serialize] Invalid width: %d", boardSize.x);
                return false;
        }
        sizeNode.read("h", boardSize.y);
        if(boardSize.y <= 0) {
            LogWarning("[GameBoard::serialize] Invalid width: %d", boardSize.y);
            return false;
        }
    }
    if(auto cellNode = node.object("cell")) {
        cellNode.read("scale", cellScale);
        if(cellScale <= 0.f || cellScale > 1.f) {
            LogWarning("[GameBoard::serialize] Invalid cell scale: %f", cellScale);
            return false;
        }
        cellNode.read("object", cellObject);
        if(cellObject.empty()) {
            LogWarning("[GameBoard::serialize] Empty cell object name");
            return false;
        }
    } else {
        LogWarning("[GameBoard::serialize] Can't find required child node: %f", "cell");
        return false;
    }
    return true;
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

void GameBoardLogic::ET_setElemState(EntityId elemEntId, EBoardElemState newState) {
    auto elem = getElem(elemEntId);
    if(!elem) {
        assert(false && "Try set state to invalid board element");
        return;
    }
    elem->state = newState;
}

EBoardElemState GameBoardLogic::ET_getElemState(EntityId elemEntId) const {
    auto elem = getElem(elemEntId);
    if(!elem) {
        assert(false && "Try set state to invalid board element");
        return EBoardElemState::Void;
    }
    return elem->state;
}

void GameBoardLogic::ET_updateBoard() {
    doUpdate = true;
}

EntityId GameBoardLogic::ET_getElemByPos(const Vec2i& pt) const {
    for(auto& elem : elements) {
        if(elem.state == EBoardElemState::Static) {
            if(elem.box.bot < pt && pt < elem.box.top) {
                return elem.entId;
            }
        }
    }
    return InvalidEntityId;
}

EntityId GameBoardLogic::ET_getElemByBoardPos(const Vec2i& boardPt) const {
    if (auto elem = getElem(boardPt)) {
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

void GameBoardLogic::ET_onTransformChanged(const Transform& newTm) {
    (void)newTm;
    ET_onBoxResized();
}

void GameBoardLogic::ET_onBoxResized() {
    initBoardBox();
    for(auto& elem : elements) {
        if (elem.state == EBoardElemState::Moving) {
            setElemBoardPos(elem, elem.boardPt);
        } else {
            setElemBoardPos(elem, elem.boardPt);
        }
        Vec2 renderSize;
        renderSize.x = static_cast<float>(objectSize.x);
        renderSize.y = static_cast<float>(objectSize.y);
        ET_SendEvent(elem.entId, &ETRenderSimpleLogic::ET_setSize, renderSize);
    }
}

void GameBoardLogic::initBoardBox() {
    AABB2Di box;

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);
    box.bot = Vec2i(0);
    box.top = viewport;

    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getAabb2di);
    Vec2i uiBoxSize = box.getSize();

    float cellSizeX = uiBoxSize.x / static_cast<float>(boardSize.x);
    float cellSizeY = uiBoxSize.y / static_cast<float>(boardSize.y);
    cellSize = static_cast<int>(floorf(std::min(cellSizeX, cellSizeY)));
    objectSize = Vec2i(static_cast<int>(floorf(cellSize * cellScale)));
    Vec2i boardBoxSize = Vec2i(cellSize * boardSize.x, cellSize * boardSize.y);

    boardBox.bot = Vec2i(0);
    boardBox.top = boardBoxSize;
    boardBox.setCenter(box.getCenter());
}

ColorB GameBoardLogic::getElemColor(BoardElemType elemType) const {
    ColorB retCol(255, 255, 255);
    switch (elemType) {
        case BoardElemType::Red:
            retCol = ColorB(255, 0, 0);
            break;
        case BoardElemType::Blue:
            retCol = ColorB(0, 0, 255);
            break;
        case BoardElemType::Green:
            retCol = ColorB(0, 255, 0);
            break;
        case BoardElemType::Purple:
            retCol = ColorB(255, 0, 255);
            break;
        case BoardElemType::Yellow:
            retCol = ColorB(255, 255, 0);
            break;
        default:
            break;
    }
    return retCol;
}

BoardElemType GameBoardLogic::getElemType() const {
    return static_cast<BoardElemType>(Math::RandomInt(0, static_cast<int>(BoardElemType::ENUM_COUNT) - 1));
}

bool GameBoardLogic::init() {
    initBoardBox();

    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            EntityId cellObjId = InvalidEntityId;
            ET_SendEventReturn(cellObjId, &ETGameObjectManager::ET_createGameObject, cellObject.c_str());
            if(cellObjId == InvalidEntityId) {
                LogWarning("[GameBoardLogic::init] Can't spawn element");
                return false;
            }
            ET_SendEvent(getEntityId(), &ETGameObject::ET_addChild, cellObjId);
            BoardElement elem;
            elem.entId = cellObjId;
            initNewElem(elem, Vec2i(i, j));
            elements.push_back(elem);
        }
    }

    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETUIBoxEvents>::connect(getEntityId());
    ETNode<ETGameObjectEvents>::connect(getEntityId());
    ETNode<ETGameBoard>::connect(getEntityId());
    return true;
}

int GameBoardLogic::getElemId(const Vec2i& boardPt) const {
    for(size_t i = 0, sz = elements.size(); i < sz; ++i) {
        if(elements[i].boardPt == boardPt) {
            return static_cast<int>(i);
        }
    }
    return INVALID_BOARD_ELEM_ID;
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

Vec3 GameBoardLogic::getPosFromBoardPos(const Vec2i& boardPt) const {
    Vec2 pt = Vec2(static_cast<float>(boardBox.bot.x), static_cast<float>(boardBox.bot.y));
    pt.x += cellSize * (boardPt.x + 0.5f);
    pt.y += cellSize * (boardPt.y + 0.5f);
    return Vec3(pt, 0.f);
}

void GameBoardLogic::setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const {
    elem.box.bot = Vec2i(static_cast<int>(boardBox.bot.x + cellSize * boardPt.x),
        static_cast<int>(boardBox.bot.y + cellSize * boardPt.y));
    elem.box.top = elem.box.bot + Vec2i(static_cast<int>(cellSize));
    elem.boardPt = boardPt;

    Transform tm;
    tm.pt = getPosFromBoardPos(boardPt);
    tm.scale = Vec3(1.f);
    ET_SendEvent(elem.entId, &ETGameObject::ET_setTransform, tm);
}

bool GameBoardLogic::isElemMatch(int firstElemId, int secondElemId) const {
    const BoardElement& firstElem = elements[firstElemId];
    const BoardElement& secondElem = elements[secondElemId];
    if(firstElem.state == EBoardElemState::Static && secondElem.state == EBoardElemState::Static) {
        return firstElem.color == secondElem.color;
    }
    return false;
}

void GameBoardLogic::initNewElem(BoardElement& elem, const Vec2i& boardPt) const {
    elem.state = EBoardElemState::Static;
    elem.movePt = boardPt;
    elem.color = getElemType();
    setElemBoardPos(elem, boardPt);

    ET_SendEvent(elem.entId, &ETRenderSimpleLogic::ET_setColor, getElemColor(elem.color));
    Vec2 renderSize;
    renderSize.x = static_cast<float>(objectSize.x);
    renderSize.y = static_cast<float>(objectSize.y);
    ET_SendEvent(elem.entId, &ETRenderSimpleLogic::ET_setSize, renderSize);
}

void GameBoardLogic::markForRemoveElems(const std::vector<int>& elems) {
    for(auto elemId : elems) {
        auto& elem = elements[elemId];
        elem.state = EBoardElemState::Void;
        ET_SendEvent(getEntityId(), &ETGameBoardElemDestroy::ET_destroyBoardElem, elem.entId);
    }
}

bool GameBoardLogic::removeHorizontalLine(const Vec2i& boardPt, int lineLen) {
    assert(lineLen >= 1 && "Invalid line len");
    std::vector<int> lineElemIds;
    lineElemIds.reserve(lineLen);
    int firstElemId = getElemId(boardPt);
    lineElemIds.push_back(firstElemId);
    for(int i = 1; i < lineLen; ++i) {
        auto elemId = getElemId(Vec2i(boardPt.x + i, boardPt.y));
        if(elemId == INVALID_BOARD_ELEM_ID) {
            return false;
        }
        if(!isElemMatch(firstElemId, elemId)) {
            return false;
        }
        lineElemIds.push_back(elemId);
    }
    markForRemoveElems(lineElemIds);
    return true;
}

bool GameBoardLogic::removeVerticalLine(const Vec2i& boardPt, int lineLen) {
    assert(lineLen >= 1 && "Invalid line len");
    std::vector<int> lineElemIds;
    lineElemIds.reserve(lineLen);
    int firstElemId = getElemId(boardPt);
    lineElemIds.push_back(firstElemId);
    for(int i = 1; i < lineLen; ++i) {
        auto elemId = getElemId(Vec2i(boardPt.x, boardPt.y + i));
        if(elemId == INVALID_BOARD_ELEM_ID) {
            return false;
        }
        if(!isElemMatch(firstElemId, elemId)) {
            return false;
        }
        lineElemIds.push_back(elemId);
    }
    markForRemoveElems(lineElemIds);
    return true;
}

const BoardElement* GameBoardLogic::getTopElem(const Vec2i& boardPt) const {
    const BoardElement* topElem = nullptr;
    for(auto& elem: elements) {
        if(elem.boardPt.x == boardPt.x) {
            if(!topElem || topElem->boardPt.y < elem.boardPt.y) {
                topElem = &elem;
            }
        }
    }
    assert(topElem && "Invalid top board element");
    return topElem;
}

int GameBoardLogic::getVoidElemBelow(const Vec2i& boardPt) const {
    auto count = 0;
    for(int i = 0; i < boardPt.y; ++i) {
        auto elem = getElem(Vec2i(boardPt.x, i));
        if(!elem || elem->state == EBoardElemState::Void) {
            ++count;
        }
    }
    return count;
}

void GameBoardLogic::updateAfterRemoves() {
    for(auto& elem : elements) {
        if(elem.state == EBoardElemState::Static || elem.state == EBoardElemState::Moving) {
            int voidBelow = getVoidElemBelow(elem.boardPt);
            if(voidBelow > 0) {
                elem.state = EBoardElemState::Moving;
                elem.movePt = Vec2i(elem.boardPt.x, elem.boardPt.y - voidBelow);
            }
        } else if(elem.state == EBoardElemState::Void) {
            auto topElem = getTopElem(elem.boardPt);
            Vec2i topPt = topElem->boardPt;
            topPt.y += 1;
            initNewElem(elem, topPt);
            int voidBelow = getVoidElemBelow(elem.boardPt);
            elem.state = EBoardElemState::Moving;
            elem.movePt = Vec2i(topPt.x, topPt.y - voidBelow);

            Transform topTm;
            ET_SendEventReturn(topTm, topElem->entId, &ETGameObject::ET_getTransform);
            topTm.pt.y += cellSize;
            topTm.scale = Vec3(1.f);
            ET_SendEvent(elem.entId, &ETGameObject::ET_setTransform, topTm);
        }
    }
}

void GameBoardLogic::updateBoard() {
    for(auto& elem : elements) {
        removeVerticalLine(elem.boardPt, 3);
        removeHorizontalLine(elem.boardPt, 3);
    }
    updateAfterRemoves();
}

Vec2i GameBoardLogic::getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const {
    Vec2i resPt;
    resPt.x = boardPt.x;
    Vec3 pos = pt - getPosFromBoardPos(boardPt);
    float yShift = pos.y;
    yShift = yShift / cellSize;
    int shift = static_cast<int>(yShift);
    if(std::abs(yShift - static_cast<float>(shift)) > 0.5f) {
        shift > 0 ? ++shift : --shift;
    }
    resPt.y = std::max(0, boardPt.y + shift);
    return resPt;
}

bool GameBoardLogic::moveElem(BoardElement& elem, float dt) {
    Transform tm;
    ET_SendEventReturn(tm, elem.entId, &ETGameObject::ET_getTransform);
    tm.pt.y -= moveSpeed * dt * cellSize;
    Vec3 desirePt = getPosFromBoardPos(elem.movePt);
    if(tm.pt.y > desirePt.y) {
        elem.boardPt = getBoardPosFromPos(elem.boardPt, tm.pt);
        ET_SendEvent(elem.entId, &ETGameObject::ET_setTransform, tm);
        return true;
    } else {
        elem.state = EBoardElemState::Static;
        setElemBoardPos(elem, elem.movePt);
        return false;
    }
}

void GameBoardLogic::ET_onTick(float dt) {
    for(auto& elem : elements) {
        if(elem.state == EBoardElemState::Moving) {
            if(!moveElem(elem, dt)) {
                doUpdate = true;
                continue;
            }
        }
    }
    if(doUpdate) {
        updateBoard();
    }
    doUpdate = false;
}