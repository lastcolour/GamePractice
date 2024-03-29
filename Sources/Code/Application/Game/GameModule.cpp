#include "Game/GameModule.hpp"
#include "Game/GameTimer.hpp"
#include "Game/GameStateManager.hpp"
#include "Game/GameMusicManager.hpp"
#include "Game/Progression/LevelProgressionSystem.hpp"
#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Game/Logics/GameBoardLogic.hpp"
#include "Game/Logics/GameBoardInteractionLogic.hpp"
#include "Game/Logics/GameScoreUpdaterLogic.hpp"
#include "Game/Logics/GameBoardMatchLogic.hpp"
#include "Game/Logics/GameBoardSpawner.hpp"
#include "Game/Logics/BoardElemDestroyAnimation.hpp"
#include "Game/Logics/BoardElemSelectAnimation.hpp"
#include "Game/Logics/GameBoardElemLandAnimation.hpp"
#include "Game/Logics/ScoreObjective.hpp"
#include "Game/Logics/GameBoardAnimation.hpp"
#include "Game/Logics/GameBoardElemsPool.hpp"
#include "Game/Logics/GameElemsPoolSetup.hpp"
#include "Game/Logics/GameBoardElemHighlighter.hpp"
#include "Game/Logics/BoardElemMergeManager.hpp"
#include "Game/Logics/BoardElemTriggerManager.hpp"
#include "Game/Logics/GameElemRocketLogic.hpp"
#include "Game/Logics/GameElemBombLogic.hpp"
#include "Game/Logics/GameElemStarLogic.hpp"
#include "Game/Logics/GameMovesLimitLogic.hpp"
#include "Game/Logics/GameTimeLimitLogic.hpp"
#include "Game/Logics/GameBoardEffects.hpp"
#include "Game/Logics/GameBoardShakeLogic.hpp"
#include "Game/ViewScripts/GameResultViewScript.hpp"
#include "Game/ViewScripts/GameViewScript.hpp"
#include "Game/ViewScripts/LevelsViewScript.hpp"
#include "Game/ViewScripts/MainViewScript.hpp"
#include "Game/ViewScripts/PauseViewScript.hpp"
#include "Game/Progression/LevelButtonList.hpp"
#include "Game/Progression/LevelButton.hpp"
#include "Entity/EntityLogicsRegister.hpp"

GameModule::LogicsContainerPtrT GameModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameTimer,
            GameStateManager,
            GameMusicManager,
            LevelProgressionSystem>()
        );
    return container;
}

void GameModule::createSystemConfigs() const {
}

void GameModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<GameBoardLogic>();
    logicsRegister.registerLogic<GameBoardElemLogic>();
    logicsRegister.registerLogic<GameBoardInteractionLogic>();
    logicsRegister.registerLogic<GameScoreUpdaterLogic>();
    logicsRegister.registerLogic<GameBoardMatchLogic>();
    logicsRegister.registerLogic<GameBoardSpawner>();
    logicsRegister.registerLogic<BoardElemDestroyAnimation>();
    logicsRegister.registerLogic<BoardElemSelectAnimation>();
    logicsRegister.registerLogic<ScoreObjective>();
    logicsRegister.registerLogic<GameBoardAnimation>();
    logicsRegister.registerLogic<GameBoardElemsPool>();
    logicsRegister.registerLogic<GameBoardElemLandAnimation>();
    logicsRegister.registerLogic<GameBoardElemHighlighter>();
    logicsRegister.registerLogic<BoardElemMergeManager>();
    logicsRegister.registerLogic<GameElemsPoolSetup>();
    logicsRegister.registerLogic<BoardElemTriggerManager>();
    logicsRegister.registerLogic<GameElemRocketLogic>();
    logicsRegister.registerLogic<GameElemBombLogic>();
    logicsRegister.registerLogic<GameElemStarLogic>();
    logicsRegister.registerLogic<GameMovesLimitLogic>();
    logicsRegister.registerLogic<GameTimeLimitLogic>();
    logicsRegister.registerLogic<GameBoardEffects>();
    logicsRegister.registerLogic<GameBoardShakeLogic>();

    logicsRegister.registerLogic<GameResultViewScript>();
    logicsRegister.registerLogic<GameViewScript>();
    logicsRegister.registerLogic<LevelsViewScript>();
    logicsRegister.registerLogic<MainViewScript>();
    logicsRegister.registerLogic<PauseViewScript>();

    logicsRegister.registerLogic<LevelButtonList>();
    logicsRegister.registerLogic<LevelButton>();
}