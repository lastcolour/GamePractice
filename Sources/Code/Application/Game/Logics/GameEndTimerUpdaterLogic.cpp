#include "Game/Logics/GameEndTimerUpdaterLogic.hpp"
#include "UI/ETUIBox.hpp"

GameEndTimerUpdaterLogic::GameEndTimerUpdaterLogic() {
}

GameEndTimerUpdaterLogic::~GameEndTimerUpdaterLogic() {
}

void GameEndTimerUpdaterLogic::Reflect(ReflectContext& ctx) {
    ctx.classInfo<GameEndTimerUpdaterLogic>("GameEndTimerUpdater");
}

void GameEndTimerUpdaterLogic::init() {
    ETNode<ETGameEndTimerUpdater>::connect(getEntityId());

    float startTime = 0.f;
    ET_SendEventReturn(startTime, &ETGameEndTimer::ET_getInitialEndGameDuration);
    ET_setEndTime(startTime);
}

void GameEndTimerUpdaterLogic::deinit() {
}

void GameEndTimerUpdaterLogic::ET_setEndTime(float endTime) {
    std::string text = StringFormat("%.1f", endTime);
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, text.c_str());
}