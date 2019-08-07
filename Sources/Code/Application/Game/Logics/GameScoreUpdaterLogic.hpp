#ifndef __GAME_SCORE_UPDATER_LOGIC_HPP__
#define __GAME_SCORE_UPDATER_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameScoreUpdaterLogic : public GameLogic,
    public ETNode<ETGameScoreUpdater> {
public:

    GameScoreUpdaterLogic();
    virtual ~GameScoreUpdaterLogic();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameScoreUpdater
    void ET_setGameScore(int score) override;
};

#endif /* __GAME_SCORE_UPDATER_LOGIC_HPP__ */