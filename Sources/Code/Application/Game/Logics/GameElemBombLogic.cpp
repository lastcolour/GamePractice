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
    ET_SendEventReturn(startPt, &ETGameBoard::ET_getElemBoardPos, getEntityId());
    currTime = 0.f;
}

bool GameElemBombLogic::ET_update(float dt) {
    currTime += dt;
    int currRadius = static_cast<int>(speed * currTime);
    if(currRadius > radius) {
        currRadius = radius;
    }

    for(int i = 1; i < radius; ++i) {
        for(int j = 1; j < radius; ++j) {
            Vec2i pt = Vec2i(i, j) + startPt;
            GameUtils::TryTriggerElemDestroy(pt);
        }
    }

    return currRadius == radius;
}