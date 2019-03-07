#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/GameETInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <algorithm>

GameBoardLogic::GameBoardLogic() :
    boardSize(0),
    space(1.f),
    cellScale(1.f),
    activeTouchEntId(InvalidEntityId) {
}

GameBoardLogic::~GameBoardLogic() {
}

bool GameBoardLogic::serialize(const JSONNode& node) {
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

bool GameBoardLogic::init(const JSONNode& node) {
    if(!serialize(node)) {
        return false;
    }
    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETRender::ET_getRenderPort);
    float cellSizeY = space * viewport.x / boardSize.x;
    float cellSizeZ = space * viewport.y / boardSize.y;
    float cellSize = std::min(cellSizeY, cellSizeZ);

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(cellSize * boardSize.x, cellSize * boardSize.y);
    aabb.setCenter(Vec2(static_cast<float>(viewport.x), static_cast<float>(viewport.y)) / 2.f);

    RenderLogicParams params;
    Transform tm;
    const float objSize = cellSize * cellScale;
    for(int i = 0; i < boardSize.x; ++i) {
        for(int j = 0; j < boardSize.y; ++j) {
            EntityId cellObjId = InvalidEntityId;
            ET_SendEventReturn(cellObjId, &ETGame::ET_createGameObject, cellObject);
            if(cellObjId != InvalidEntityId) {
                Vec2 pt = aabb.bot;
                pt.x += cellSize * (i + 0.5f);
                pt.y += cellSize * (j + 0.5f);

                tm.pt = Vec3(pt, 0.f);
                ET_SendEvent(cellObjId, &ETGameObject::ET_setTransform, tm);

                params.col = ColorF(1.f, 1.f, 0.f);
                params.size = Vec2(objSize);
                ET_SendEvent(cellObjId, &ETRenderLogic::ET_setRenderParams, params);

                ET_SendEvent(cellObjId, &ETGameBoardElemLogic::ET_setBoardPos, Vec2i(i, j));

                TouchAaabb touchBox;
                touchBox.entId = cellObjId;
                touchBox.box.bot = Vec2i(static_cast<int>(aabb.bot.x + cellSize * i),
                    static_cast<int>(aabb.bot.y + cellSize * j));
                touchBox.box.top = touchBox.box.bot + Vec2i(static_cast<int>(cellSize));
                touchMap.push_back(touchBox);
            } else {
                LogWarning("[GameBoard::init] Can't create cell object: %s", cellObject);
                return false;
            }
        }
    }

    ETNode<ETSurfaceEvents>::connect(getEntityId());
    return true;
}

EntityId GameBoardLogic::findTouchedEntity(const Vec2i& pt) const {
    for(const auto& item : touchMap) {
        const auto& box = item.box;
        if(pt > box.bot && pt < box.top) {
            return item.entId;
        }
    }
    return InvalidEntityId;
}

void GameBoardLogic::ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) {
    if(touchType == ETouchType::Press) {
        auto touchedEntId = findTouchedEntity(pt);
        if(touchedEntId != InvalidEntityId) {
            activeTouchEntId = touchedEntId;
        }
    } else if (activeTouchEntId == InvalidEntityId || touchType != ETouchType::Move) {
        return;
    }
    Vec2i boardPt(-1);
    ET_SendEventReturn(boardPt, activeTouchEntId, &ETGameBoardElemLogic::ET_getBoardPos);
    Vec2i nextBoardPt = boardPt;
    Transform tm;
    ET_SendEventReturn(tm, &ETGameObject::ET_getTransform);
    Vec2 moveDir = Vec2(tm.pt.x, tm.pt.y) - Vec2(static_cast<float>(pt.x), static_cast<float>(pt.y));
    if(std::abs(moveDir.x) > std::abs(moveDir.y)) {
        if(moveDir.x > 0.f) {
            nextBoardPt.x = boardPt.x + 1;
        } else {
            nextBoardPt.x = boardPt.x - 1;
        }
    } else {
        if(moveDir.y > 0.f) {
            nextBoardPt.y = boardPt.y + 1;
        } else {
            nextBoardPt.y = boardPt.y - 1;
        }
    }
    if(nextBoardPt < boardSize || nextBoardPt >= boardSize) {
        return;
    }
    if(moveDir.getLenght() > space * 0.6f) {
        LogError("[Switch]");
    }
}