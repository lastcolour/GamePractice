#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/GameETInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <algorithm>
#include <cassert>

namespace {
    const int INVALID_BOARD_ELEM_ID = -1;
    const float MIN_MOVE_LEN_FOR_SWITCH = 0.6f;
} // namespace

GameBoardLogic::GameBoardLogic() :
    boardSize(0),
    space(1.f),
    cellScale(1.f),
    activeTouchedElemId(INVALID_BOARD_ELEM_ID) {
}

GameBoardLogic::~GameBoardLogic() {
}

bool GameBoardLogic::serialize(const JSONNode& node) {
    node.value("fallSpeed", moveSpeed);
    if(moveSpeed <= 0.f) {
        LogWarning("[GameBoard::serialize] Invalid move speed: %f", moveSpeed);
        return false;
    }
    node.value("space", space);
    if(space <= 0.f || space > 1.f) {
        LogWarning("[GameBoard::serialize] Invalid space: %f", space);
        return false;
    }
    if(auto sizeNode = node.object("size")) {
        sizeNode.value("w", boardSize.x);
        if(boardSize.x <= 0) {
            LogWarning("[GameBoard::serialize] Invalid width: %d", boardSize.x);
            return false;
        }
        sizeNode.value("h", boardSize.y);
        if(boardSize.y <= 0) {
            LogWarning("[GameBoard::serialize] Invalid width: %d", boardSize.y);
            return false;
        }
    }
    if(auto cellNode = node.object("cell")) {
        cellNode.value("scale", cellScale);
        if(cellScale <= 0.f || cellScale > 1.f) {
            LogWarning("[GameBoard::serialize] Invalid cell scale: %f", cellScale);
            return false;
        }
        cellNode.value("object", cellObject);
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

ColorF GameBoardLogic::getElemColor(BoardElemType elemType) const {
    ColorF retCol(1.f, 1.f, 1.f);
    switch (elemType) {
        case BoardElemType::Red:
            retCol = ColorF(1.f, 0.f, 0.f);
            break;
        case BoardElemType::Blue:
            retCol = ColorF(0.f, 0.f, 1.f);
            break;
        case BoardElemType::Green:
            retCol = ColorF(0.f, 1.f, 0.f);
            break;
        case BoardElemType::Purple:
            retCol = ColorF(1.f, 0.f, 1.f);
            break;
        case BoardElemType::Yellow:
            retCol = ColorF(1.f, 1.f, 0.f);
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
    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRender::ET_getRenderPort);
    float cellSizeY = space * viewport.x / boardSize.x;
    float cellSizeZ = space * viewport.y / boardSize.y;
    cellSize = std::min(cellSizeY, cellSizeZ);

    boardBox.bot = Vec2(0.f);
    boardBox.top = Vec2(cellSize * boardSize.x, cellSize * boardSize.y);
    boardBox.setCenter(Vec2(static_cast<float>(viewport.x), static_cast<float>(viewport.y)) / 2.f);
    objectSize = cellSize * cellScale;

    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            EntityId cellObjId = InvalidEntityId;
            ET_SendEventReturn(cellObjId, &ETGame::ET_createGameObject, cellObject);
            if(cellObjId == InvalidEntityId) {
                LogWarning("[GameBoardLogic::init] Can't spawn element");
                return false;
            }
            BoardElement elem;
            elem.entId = cellObjId;
            initNewElem(elem, Vec2i(i, j));
            elements.push_back(elem);
        }
    }

    ETNode<ETSurfaceEvents>::connect(getEntityId());
    ETNode<ETGameTick>::connect(getEntityId());
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

BoardElement* GameBoardLogic::getElem(const Vec2i& boardPt) {
    for(auto& elem : elements) {
        if(elem.boardPt == boardPt) {
            return &elem;
        }
    }
    return nullptr;
}

int GameBoardLogic::findTouchedElemId(const Vec2i& pt) const {
    for(size_t i = 0, sz = elements.size(); i < sz; ++i) {
        const auto& boardElem = elements[i];
        if(boardElem.state == BoardElemState::Static) {
            if(boardElem.box.bot < pt && pt < boardElem.box.top) {
                return static_cast<int>(i);
            }
        }
    }
    return INVALID_BOARD_ELEM_ID;
}

void GameBoardLogic::onStartElemMove() {
}

void GameBoardLogic::onElemMove() {
}

Vec3 GameBoardLogic::getPosFromBoardPos(const Vec2i& boardPt) const {
    Vec2 pt = boardBox.bot;
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
    tm.pt = getPosFromBoardPos(elem.boardPt);
    ET_SendEvent(elem.entId, &ETGameObject::ET_setTransform, tm);
}

void GameBoardLogic::switchElements(int firstElemId, int secondElemId) {
    assert(firstElemId != secondElemId && "Can't switch the same element");
    BoardElement& firstElem = elements[firstElemId];
    BoardElement& secondElem = elements[secondElemId];
    std::swap(firstElem.boardPt, secondElem.boardPt);
    setElemBoardPos(firstElem, firstElem.boardPt);
    setElemBoardPos(secondElem, secondElem.boardPt);
}

bool GameBoardLogic::isElemMatch(int firstElemId, int secondElemId) const {
    const BoardElement& firstElem = elements[firstElemId];
    const BoardElement& secondElem = elements[secondElemId];
    if(firstElem.state == BoardElemState::Static && secondElem.state == BoardElemState::Static) {
        return firstElem.color == secondElem.color;
    }
    return false;
}

void GameBoardLogic::initNewElem(BoardElement& elem, const Vec2i& boardPt) const {
    elem.state = BoardElemState::Static;
    elem.movePt = boardPt;
    elem.color = getElemType();
    elem.state = BoardElemState::Static;
    setElemBoardPos(elem, boardPt);

    RenderLogicParams params;
    params.col = getElemColor(elem.color);
    params.size = Vec2(objectSize);
    ET_SendEvent(elem.entId, &ETRenderLogic::ET_setRenderParams, params);
}

void GameBoardLogic::markForRemoveElems(const std::vector<int>& elems) {
    for(auto elemId : elems) {
        auto& elem = elements[elemId];
        elem.state = BoardElemState::Void;
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
        if(!elem || elem->state == BoardElemState::Void) {
            ++count;
        }
    }
    return count;
}

void GameBoardLogic::updateAfterRemoves() {
    for(auto& elem : elements) {
        if(elem.state == BoardElemState::Static || elem.state == BoardElemState::Moving) {
            int voidBelow = getVoidElemBelow(elem.boardPt);
            if(voidBelow > 0) {
                elem.state = BoardElemState::Moving;
                elem.movePt = Vec2i(elem.boardPt.x, elem.boardPt.y - voidBelow);
            }
        } else if(elem.state == BoardElemState::Void) {
            auto topElem = getTopElem(elem.boardPt);
            Vec2i topPt = topElem->boardPt;
            topPt.y += 1;
            initNewElem(elem, topPt);
            int voidBelow = getVoidElemBelow(elem.boardPt);
            elem.state = BoardElemState::Moving;
            elem.movePt = Vec2i(topPt.x, topPt.y - voidBelow);

            Transform topTm;
            ET_SendEventReturn(topTm, topElem->entId, &ETGameObject::ET_getTransform);
            topTm.pt.y += cellSize;
            ET_SendEvent(elem.entId, &ETGameObject::ET_setTransform, topTm);
        }
    }
}

void GameBoardLogic::updateBoard() {
    bool wasRemove = false;
    for(auto& elem : elements) {
        wasRemove |= removeVerticalLine(elem.boardPt, 3);
        wasRemove |= removeHorizontalLine(elem.boardPt, 3);
    }
    if(wasRemove) {
        updateAfterRemoves();
    }
}

void GameBoardLogic::onEndElemMove(const Vec2i& endPt) {
    BoardElement& elem = elements[activeTouchedElemId];
    Vec2i nextBoardPt = elem.boardPt;
    Transform tm;
    ET_SendEventReturn(tm, elem.entId, &ETGameObject::ET_getTransform);
    Vec2 moveDir = Vec2(static_cast<float>(endPt.x), static_cast<float>(endPt.y)) - Vec2(tm.pt.x, tm.pt.y);
    if(std::abs(moveDir.x) > std::abs(moveDir.y)) {
        if(moveDir.x > 0.f) {
            nextBoardPt.x = elem.boardPt.x + 1;
        } else {
            nextBoardPt.x = elem.boardPt.x - 1;
        }
    } else {
        if(moveDir.y > 0.f) {
            nextBoardPt.y = elem.boardPt.y + 1;
        } else {
            nextBoardPt.y = elem.boardPt.y - 1;
        }
    }
    if(moveDir.getLenght() < cellSize * MIN_MOVE_LEN_FOR_SWITCH) {
        return;
    }
    if(nextBoardPt < Vec2i(0) || nextBoardPt >= boardSize) {
        return;
    }
    int nextElemId = getElemId(nextBoardPt);
    if(nextElemId != INVALID_BOARD_ELEM_ID) {
        switchElements(activeTouchedElemId, nextElemId);
        updateBoard();
    }
}

void GameBoardLogic::ET_onSurfaceResize(const Vec2i& pt) {
    (void)pt;
    return;
}

void GameBoardLogic::ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) {
    if(touchType == ETouchType::Press) {
        int touchedElemId = findTouchedElemId(pt);
        if(touchedElemId != INVALID_BOARD_ELEM_ID) {
            activeTouchedElemId = touchedElemId;
            onStartElemMove();
        }
    } else if(activeTouchedElemId != INVALID_BOARD_ELEM_ID && touchType == ETouchType::Move) {
        onElemMove();
    } else if(activeTouchedElemId != INVALID_BOARD_ELEM_ID && touchType == ETouchType::Release) {
        onEndElemMove(pt);
        activeTouchedElemId = INVALID_BOARD_ELEM_ID;
    }
}

Vec2i GameBoardLogic::getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const {
    Vec2i resPt;
    resPt.x = boardPt.x;
    Vec3 pos = pt - getPosFromBoardPos(boardPt);
    float yShift = pos.y;
    yShift = yShift / cellSize;
    int shift = static_cast<int>(yShift);
    if(std::abs(yShift - static_cast<float>(shift)) > 0.5f) {
        shift = shift > 0 ? ++shift : --shift; 
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
        elem.state = BoardElemState::Static;
        setElemBoardPos(elem, elem.movePt);
        return false;
    }
}

void GameBoardLogic::ET_onGameTick(float dt) {
    bool moveFinished = false;
    for(auto& elem : elements) {
        if(elem.state == BoardElemState::Moving) {
            if(!moveElem(elem, dt)) {
                moveFinished = true;
                continue;
            }
        }
    }
    if(moveFinished) {
        updateBoard();
    }
}