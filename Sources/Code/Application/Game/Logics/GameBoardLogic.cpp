#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/ETLogger.hpp"
#include "Render/ETRenderNode.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntityManger.hpp"

#include <algorithm>
#include <cassert>

namespace {

const int INVALID_BOARD_ELEM_ID = -1;

} // namespace

GameBoardLogic::GameBoardLogic() :
    boardSize(0),
    cellScale(1.f),
    doUpdate(false) {
}

GameBoardLogic::~GameBoardLogic() {
}

void GameBoardLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardLogic>("GameBoard")) {
        classInfo->addField("fallSpeed", &GameBoardLogic::moveSpeed);
        classInfo->addField("size", &GameBoardLogic::boardSize);
        classInfo->addField("cellObject", &GameBoardLogic::cellObject);
        classInfo->addField("cellScale", &GameBoardLogic::cellScale);
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
        assert(false && "Try get state from invalid board element");
        return EBoardElemState::Void;
    }
    return elem->state;
}

EBoardElemType GameBoardLogic::ET_getElemType(EntityId elemEntId) const {
    auto elem = getElem(elemEntId);
    if(!elem) {
        assert(false && "Try get type from invalid board element");
        return EBoardElemType::None;
    }
    return elem->type;
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

void GameBoardLogic::ET_onTransformChanged(const Transform& newTm) {
    (void)newTm;
    ET_onBoxResized();
}

void GameBoardLogic::ET_onBoxResized() {
    initBoardBox();
    for(auto& elem : elements) {
        if(elem.state == EBoardElemState::Moving) {
            setElemBoardPos(elem, elem.boardPt);
        } else {
            setElemBoardPos(elem, elem.boardPt);
        }
        ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
    }
}

void GameBoardLogic::initBoardBox() {
    AABB2Di box;

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRenderCamera::ET_getRenderPort);
    box.bot = Vec2i(0);
    box.top = viewport;

    ET_SendEventReturn(box, getEntityId(), &ETUIBox::ET_getBox);
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

ColorB GameBoardLogic::getElemColor(EBoardElemType elemType) const {
    ColorB retCol(255, 255, 255);
    switch(elemType) {
        case EBoardElemType::Red:
            retCol = ColorB(255, 0, 0);
            break;
        case EBoardElemType::Blue:
            retCol = ColorB(0, 0, 255);
            break;
        case EBoardElemType::Green:
            retCol = ColorB(0, 255, 0);
            break;
        case EBoardElemType::Purple:
            retCol = ColorB(255, 0, 255);
            break;
        case EBoardElemType::Yellow:
            retCol = ColorB(255, 255, 0);
            break;
        default:
            break;
    }
    return retCol;
}

bool GameBoardLogic::init() {
    initBoardBox();

    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            EntityId cellObjId = InvalidEntityId;
            ET_SendEventReturn(cellObjId, &ETEntityManager::ET_createEntity, cellObject.c_str());
            if(cellObjId == InvalidEntityId) {
                LogWarning("[GameBoardLogic::init] Can't spawn element");
                return false;
            }
            ET_SendEvent(getEntityId(), &ETEntity::ET_addChild, cellObjId);
            BoardElement elem;
            elem.entId = cellObjId;
            initNewElem(elem, Vec2i(i, j));
            elements.push_back(elem);
        }
    }

    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETUIBoxEvents>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    ETNode<ETGameBoard>::connect(getEntityId());
    return true;
}

void GameBoardLogic::deinit() {
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

Vec3 GameBoardLogic::ET_getPosFromBoardPos(const Vec2i& boardPt) const {
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
    tm.pt = ET_getPosFromBoardPos(boardPt);
    tm.scale = Vec3(1.f);
    ET_SendEvent(elem.entId, &ETEntity::ET_setTransform, tm);
}

void GameBoardLogic::setElemType(BoardElement& elem) const {
    elem.type = static_cast<EBoardElemType>(Math::RandomInt(1, static_cast<int>(EBoardElemType::Yellow)));
    ET_SendEvent(elem.entId, &ETRenderColoredTexture::ET_setTextureColor, getElemColor(elem.type));
}

void GameBoardLogic::initNewElem(BoardElement& elem, const Vec2i& boardPt) const {
    elem.state = EBoardElemState::Static;
    elem.movePt = boardPt;
    setElemBoardPos(elem, boardPt);
    setElemType(elem);

    ET_SendEvent(elem.entId, &ETRenderRect::ET_setSize, objectSize);
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
            ET_SendEventReturn(topTm, topElem->entId, &ETEntity::ET_getTransform);
            topTm.pt.y += cellSize;
            topTm.scale = Vec3(1.f);
            ET_SendEvent(elem.entId, &ETEntity::ET_setTransform, topTm);
        }
    }
}

void GameBoardLogic::updateBoard() {
    std::vector<EntityId> elemToRemove;
    ET_SendEventReturn(elemToRemove, getEntityId(), &ETGameBoardMatcher::ET_getMatchedElements);
    for(auto& elemEntId : elemToRemove) {
        auto boardElem = getElem(elemEntId);
        assert(boardElem && "Try to remove invalid board elem");
        boardElem->state = EBoardElemState::Void;
        ET_SendEvent(getEntityId(), &ETGameBoardElemDestroy::ET_destroyBoardElem, boardElem->entId);
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

bool GameBoardLogic::moveElem(BoardElement& elem, float dt) {
    Transform tm;
    ET_SendEventReturn(tm, elem.entId, &ETEntity::ET_getTransform);
    tm.pt.y -= moveSpeed * dt * cellSize;
    Vec3 desirePt = ET_getPosFromBoardPos(elem.movePt);
    if(tm.pt.y > desirePt.y) {
        elem.boardPt = getBoardPosFromPos(elem.boardPt, tm.pt);
        ET_SendEvent(elem.entId, &ETEntity::ET_setTransform, tm);
        return true;
    } else {
        elem.state = EBoardElemState::Static;
        setElemBoardPos(elem, elem.movePt);
        return false;
    }
}

void GameBoardLogic::ET_onGameTick(float dt) {
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

const Vec2i& GameBoardLogic::ET_getBoardSize() const {
    return boardSize;
}

const AABB2Di& GameBoardLogic::ET_getBoardBox() const {
    return boardBox;
}