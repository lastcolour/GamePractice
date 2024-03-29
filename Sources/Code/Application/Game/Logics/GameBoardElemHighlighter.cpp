#include "Game/Logics/GameBoardElemHighlighter.hpp"
#include "Render/ETRenderNode.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Game/Logics/GameBoardUtils.hpp"
#include "Render/ETRenderScene.hpp"

#include <cassert>

void GameBoardElemHighlighter::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardElemHighlighter>("GameBoardElemHighlighter")) {
        classInfo->addField("highlightEntity", &GameBoardElemHighlighter::highlightEntityRes);
        classInfo->addField("fadeOutDuration", &GameBoardElemHighlighter::fadeOutDuration);
        classInfo->addField("cellScale", &GameBoardElemHighlighter::cellScale);
    }
}

GameBoardElemHighlighter::GameBoardElemHighlighter() :
    fadeOutDuration(0.1f),
    cellScale(0.95f) {
}

GameBoardElemHighlighter::~GameBoardElemHighlighter() {
}

void GameBoardElemHighlighter::init() {
    if(!createElemsPool()) {
        return;
    }
    ETNode<ETGameBoardElemHighlighter>::connect(getEntityId());
    ETNode<ETGameTimerEvents>::connect(getEntityId());
    ETNode<ETGameBoardSpawnerEvents>::connect(getEntityId());
}

void GameBoardElemHighlighter::deinit() {
    destroyAllElems();
}

void GameBoardElemHighlighter::ET_onGameTick(float dt) {
    for(auto& elem : elements) {
        updateElem(elem, dt);
    }
}

void GameBoardElemHighlighter::ET_highlightCell(const Vec2i& cellPt) {
    Vec2i boardSize(0);
    ET_SendEventReturn(boardSize, &ETGameBoard::ET_getBoardSize);
    if(!(cellPt >= Vec2i(0) && cellPt < boardSize)) {
        return;
    }

    HighlightElem* freeElem = nullptr;
    for(auto& elem : elements) {
        if(elem.boardPt == cellPt) {
            freeElem = &elem;
            break;
        }
    }
    if(!freeElem) {
        for(auto& elem : elements) {
            if(elem.state == State::Finished) {
                freeElem = &elem;
                break;
            }
        }
    }
    if(!freeElem) {
        LogError("[GameBoardElemHighlighter::ET_highlightCell] Can't find free elem to highlight cell: (%d, %d)",
            cellPt.x, cellPt.y);
        return;
    }

    if(freeElem->state == State::Finished) {
        ET_SendEvent(rootRenderId, &ETRenderScene::ET_addItem, freeElem->entId, GameUtils::ELEM_HIGHLIGHT_Z_OFFSET);
    }

    freeElem->boardPt = cellPt;
    freeElem->duration = 0.f;
    freeElem->state = State::FadeOut;

    if(!rootRenderId.isValid()) {
        ET_SendEventReturn(rootRenderId, &ETGameBoard::ET_getRootRenderId);
    }

    Transform tm;
    ET_SendEventReturn(tm.pt, &ETGameBoard::ET_getPosFromBoardPos, cellPt);
    ET_SendEvent(freeElem->entId, &ETEntity::ET_setLocalTransform, tm);

    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    float elemSize = cellSize * cellScale;
    ET_SendEvent(freeElem->entId, &ETRenderRect::ET_setSize, Vec2(elemSize));
    ET_SendEvent(freeElem->entId, &ETRenderNode::ET_show);
}

bool GameBoardElemHighlighter::createElemsPool() {
    destroyAllElems();
    int poolSize = 10 * 10;
    for(int i = 0; i < poolSize; ++i) {
        HighlightElem elem;
        elem.entId = highlightEntityRes.createInstance();
        if(!elem.entId.isValid()) {
            LogError("[GameBoardElemHighlighter::init] Can't create entity: '%s'", highlightEntityRes.getPath());
            destroyAllElems();
            return false;
        }

        ET_SendEvent(elem.entId, &ETEntity::ET_setParent, getEntityId());

        elem.state = State::Finished;
        elem.duration = 0.f;
        elem.boardPt = Vec2i(-1);
        elements.emplace_back(elem);

        ET_SendEvent(elem.entId, &ETRenderNode::ET_hide);

    }
    return true;
}

void GameBoardElemHighlighter::destroyAllElems() {
    for(auto& elem : elements) {
        ET_SendEvent(rootRenderId, &ETRenderScene::ET_removeItem, elem.entId);
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, elem.entId);
    }
    elements.clear();
}

void GameBoardElemHighlighter::updateElem(HighlightElem& elem, float dt) {
    if(elem.state == State::Finished) {
        return;
    }
    elem.duration += dt;

    const float prog = Math::Clamp(elem.duration / fadeOutDuration, 0.f, 1.f);
    const float resAlpha = Math::Lerp(1.f, 0.f, prog);

    ET_SendEvent(elem.entId, &ETRenderNode::ET_setAlphaMultiplier, resAlpha);

    if(elem.duration > fadeOutDuration) {
        elem.state = State::Finished;
        elem.boardPt = Vec2i(-1);
        ET_SendEvent(rootRenderId, &ETRenderScene::ET_removeItem, elem.entId);
        ET_SendEvent(elem.entId, &ETRenderNode::ET_hide);
    }
}

void GameBoardElemHighlighter::ET_onStartLoading() {
}

void GameBoardElemHighlighter::ET_onStartDestroying() {
    ET_onGameTick(1024.f);
    rootRenderId = InvalidEntityId;
}