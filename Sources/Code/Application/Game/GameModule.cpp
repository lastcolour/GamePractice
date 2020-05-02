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

GameModule::LogicsContainerPtrT GameModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameTimer,
            GameStateManager>()
        );
    return container;
}

GameModule::ConfigsPtrT GameModule::getSystemConfigs() const {
    return nullptr;
}

void GameModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
    logicRegister.registerLogic<GameBoardLogic>("GameBoard");
    logicRegister.registerLogic<GameBoardElemLogic>("GameBoardElem");
    logicRegister.registerLogic<GameEndTimerLogic>("GameEndTimer");
    logicRegister.registerLogic<GameEndTimerUpdaterLogic>("GameEndTimerUpdater");
    logicRegister.registerLogic<GameBoardElemSwitcherLogic>("GameBoardElemSwitcher");
    logicRegister.registerLogic<GameBoardInteractionLogic>("GameBoardInteraction");
    logicRegister.registerLogic<GameBoardElemDestroyLogic>("GameBoardElemDestroy");
    logicRegister.registerLogic<GameScoreLogic>("GameScore");
    logicRegister.registerLogic<GameScoreUpdaterLogic>("GameScoreUpdater");
    logicRegister.registerLogic<GameBoardAppearAnimationLogic>("GameBoardAppearAnimation");
    logicRegister.registerLogic<GameBoardMatchLogic>("GameBoardMatcher");
}