#include "Game/GameModule.hpp"
#include "Game/GameTimer.hpp"
#include "Game/GameStateManager.hpp"
#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameEndTimerLogic.hpp"
#include "Game/Logics/GameEndTimerUpdaterLogic.hpp"
#include "Game/Logics/GameBoardElemSwitcherLogic.hpp"
#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/Logics/GameBoardElemDestroyLogic.hpp"
#include "Game/Logics/GameScoreLogic.hpp"
#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "Game/Logics/GameBoardAppearAnimationLogic.hpp"
#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Entity/EntityLogicRegister.hpp"

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

void GameModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
    logicRegister.registerLogic<GameBoardLogic>();
    logicRegister.registerLogic<GameBoardElemLogic>();
    logicRegister.registerLogic<GameEndTimerLogic>();
    logicRegister.registerLogic<GameEndTimerUpdaterLogic>();
    logicRegister.registerLogic<GameBoardElemSwitcherLogic>();
    logicRegister.registerLogic<GameBoardInteractionLogic>();
    logicRegister.registerLogic<GameBoardElemDestroyLogic>();
    logicRegister.registerLogic<GameScoreLogic>();
    logicRegister.registerLogic<GameScoreUpdaterLogic>();
    logicRegister.registerLogic<GameBoardAppearAnimationLogic>();
    logicRegister.registerLogic<GameBoardMatchLogic>();
}