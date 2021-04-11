#include "Game/Logics/GameElemRocketLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

namespace {

void tryTriggerDestroy(const Vec2i& boardPt) {
    EntityId elemId;
    ET_SendEventReturn(elemId, &ETGameBoard::ET_getElemByBoardPos, boardPt);
    if(!elemId.isValid()) {
        return;
    }
    auto elemState = GameUtils::GetElemState(elemId);
    if(elemState != EBoardElemState::Static) {
        return;
    }
    ET_SendEvent(elemId, &ETGameBoardElem::ET_triggerDestroy);
}

} // namespace

void GameElemRocketLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameElemRocketLogic>("GameElemRocketLogic")) {
        classInfo->addField("speed", &GameElemRocketLogic::speed);
    }
}

GameElemRocketLogic::GameElemRocketLogic() :
    startPt(-1),
    speed(1.f),
    isHorizontal(false) {
}

GameElemRocketLogic::~GameElemRocketLogic() {
}

void GameElemRocketLogic::init() {
    ETNode<ETGameBoardElemTriggerLogic>::connect(getEntityId());
}

void GameElemRocketLogic::deinit() {
}

void GameElemRocketLogic::ET_start() {
    ET_SendEventReturn(startPt, &ETGameBoard::ET_getElemBoardPos, getEntityId());

    currTime = 0.f;
    auto elemType = GameUtils::GetElemType(getEntityId());
    if(elemType == EBoardElemType::HRocket) {
        isHorizontal = true;
    } else {
        isHorizontal = false;
    }
}

bool GameElemRocketLogic::ET_update(float dt) {
    currTime += dt;
    int offset = static_cast<int>(speed * currTime);

    if(isHorizontal) {
        for(int i = 1; i < offset; ++i) {
            {
                Vec2i pt = startPt;
                pt.x += i;
                tryTriggerDestroy(pt);
            }
            {
                Vec2i pt = startPt;
                pt.x -= i;
                tryTriggerDestroy(pt);
            }
        }
    } else {
        for(int i = 1; i < offset; ++i) {
            {
                Vec2i pt = startPt;
                pt.y += i;
                tryTriggerDestroy(pt);
            }
            {
                Vec2i pt = startPt;
                pt.y -= i;
                tryTriggerDestroy(pt);
            }
        }
    }

    Vec2i size(0);
    ET_SendEventReturn(size, &ETGameBoard::ET_getBoardSize);

    bool isEnded = false;
    if(isHorizontal) {
        if((startPt.x - offset) < 0 && (startPt.x + offset) >= size.x) {
            isEnded = true;
        }
    } else {
        if((startPt.y - offset) < 0 && (startPt.y + offset) >= size.y) {
            isEnded = true;
        }
    }

    if(isEnded) {
        ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onTriggerDone);
    }

    return isEnded;
}