#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/GameETInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <algorithm>

GameBoardLogic::GameBoardLogic() :
    width(0),
    height(0),
    space(1.f),
    cellScale(1.f) {
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
        sizeNode.value("w", width);
        if(width <= 0) {
            LogWarning("[GameBoard::serialize] Invalid width: %d", width);
            return false;
        }
        sizeNode.value("h", height);
        if(height <= 0) {
            LogWarning("[GameBoard::serialize] Invalid width: %d", height);
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
    const Vec2i viewport = ET_SendEventReturn(&ETRender::ET_getRenderPort);
    float cellSizeY = space * viewport.x / width ;
    float cellSizeZ = space * viewport.y / height;
    float cellSize = std::min(cellSizeY, cellSizeZ);

    AABB2D aabb;
    aabb.bot = Vec2(0.f);
    aabb.top = Vec2(cellSize * width, cellSize * height);
    aabb.setCenter( Vec2(static_cast<float>(viewport.x), static_cast<float>(viewport.y)) / 2.f);

    RenderLogicParams params;
    const float objSize = cellSize * cellScale;
    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            auto cellObjId = ET_SendEventReturn(&ETGame::ET_createGameObject, cellObject);
            if(cellObjId != InvalidEntityId) {
                Vec2 pt = aabb.bot;
                pt.x += cellSize * (i + 0.5f);
                pt.y += cellSize * (j + 0.5f);
                params.pt = pt;
                params.col = ColorF(1.f, 1.f, 0.f);
                params.size = Vec2(objSize);
                ET_SendEvent(cellObjId, &ETRenderLogic::ET_setRenderParams, params);

                TouchAaabb touchBox;
                touchBox.entId = cellObjId;
                touchBox.box.bot = Vec2i(aabb.bot.x + cellSize * i, aabb.bot.y + cellSize * j);
                touchBox.box.top = touchBox.box.bot + Vec2i(cellSize);
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

void GameBoardLogic::ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) {
    for(const auto& item : touchMap) {
        const auto& box = item.box;
        if(pt > box.bot && pt < box.top) {
            RenderLogicParams params;
            ET_SendEvent(item.entId, &ETRenderLogic::ET_getRenderParams, params);
            params.col = ColorF(1.f, 0.f, 0.f);
            ET_SendEvent(item.entId, &ETRenderLogic::ET_setRenderParams, params);
        }
    }
}