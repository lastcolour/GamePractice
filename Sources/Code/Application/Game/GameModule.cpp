#include "Game/GameModule.hpp"
#include "Game/GameTimer.hpp"
#include "Game/GameStateManager.hpp"
#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameEndTimerLogic.hpp"
#include "Game/Logics/GameEndTimerUpdaterLogic.hpp"
#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/Logics/GameScoreLogic.hpp"
#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "Game/Logics/GameBoardAppearAnimationLogic.hpp"
#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Game/Logics/GameBoardSpawner.hpp"
#include "Game/Logics/BoardElemDestroyAnimation.hpp"
#include "Game/Logics/BoardElemSelectAnimation.hpp"
#include "Entity/EntityLogicsRegister.hpp"

GameModule::LogicsContainerPtrT GameModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameTimer,
            GameStateManager>()
        );
    return container;
}

void GameModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void GameModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<GameBoardLogic>();
    logicsRegister.registerLogic<GameBoardElemLogic>();
    logicsRegister.registerLogic<GameEndTimerLogic>();
    logicsRegister.registerLogic<GameEndTimerUpdaterLogic>();
    logicsRegister.registerLogic<GameBoardInteractionLogic>();
    logicsRegister.registerLogic<GameScoreLogic>();
    logicsRegister.registerLogic<GameScoreUpdaterLogic>();
    logicsRegister.registerLogic<GameBoardAppearAnimationLogic>();
    logicsRegister.registerLogic<GameBoardMatchLogic>();
    logicsRegister.registerLogic<GameBoardSpawner>();
    logicsRegister.registerLogic<BoardElemDestroyAnimation>();
    logicsRegister.registerLogic<BoardElemSelectAnimation>();
}