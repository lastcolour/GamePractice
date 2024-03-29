#ifndef __GAME_BOARD_FSM_HPP__
#define __GAME_BOARD_FSM_HPP__

#include "Game/ETGameElem.hpp"

enum class EGameBoardUpdatePass {
    Move = 0,
    Merge,
    Trigger,
    Match,
    Static,
    Respawn,
    Switch
};

struct GameBoardState {

    bool hasMovingElems;
    bool hasMergingElems;
    bool hasTriggeringElems;
    bool isMatchRequested;
    bool isMatchBlocked;
    bool isRespawnRequested;
    bool hasSwitchingElems;

public:

    GameBoardState();
};

class GameBoardFSM {
public:

    GameBoardFSM();
    ~GameBoardFSM();

    bool queryPass(EGameBoardUpdatePass& outPass);
    bool querySubPass(EGameBoardUpdatePass& outPass);

    GameBoardState& getState();
    const GameBoardState& getState() const;

    bool canQueryPass() const;

private:

    GameBoardState state;
    EGameBoardUpdatePass updatePass;
    EGameBoardUpdatePass returnPass;
    bool isStaticWasVisited;
};

#endif /* __GAME_BOARD_FSM_HPP__ */