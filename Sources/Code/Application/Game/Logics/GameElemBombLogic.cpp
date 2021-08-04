#include "Game/Logics/GameElemBombLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

void GameElemBombLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameElemBombLogic>("GameElemBombLogic")) {
        classInfo->addField("speed", &GameElemBombLogic::speed);
        classInfo->addField("radius", &GameElemBombLogic::radius);
    }
}

GameElemBombLogic::GameElemBombLogic() :
    startPt(-1),
    speed(1.f),
    currTime(0.f),
    radius(3) {
}

GameElemBombLogic::~GameElemBombLogic() {
}

void GameElemBombLogic::init() {
    ETNode<ETGameBoardElemTriggerLogic>::connect(getEntityId());
}

void GameElemBombLogic::deinit() {
}

void GameElemBombLogic::ET_start() {
    ET_SendEvent(&ETGameBoardShake::ET_addShake);
    ET_SendEventReturn(startPt, &ETGameBoard::ET_getElemBoardPos, getEntityId());
    currTime = 0.f;
}

void GameElemBombLogic::ET_setSwapedElem(EntityId elemId) {
}

bool GameElemBombLogic::ET_update(float dt) {
    currTime += dt;
    int currRadius = static_cast<int>(speed * currTime);
    if(currRadius > radius) {
        currRadius = radius;
    }

    for(int i = -radius; i <= radius; ++i) {
        for(int j = -radius; j <= radius; ++j) {
            if(i == 0 && j == 0) {
                continue;
            }
            Vec2i pt = startPt + Vec2i(i, j);
            GameUtils::TryTriggerElemDestroy(getEntityId(), pt);
        }
    }

    bool isEnded = currRadius >= radius;
    if(isEnded) {
        ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onTriggerDone);
    }

    return isEnded;
}