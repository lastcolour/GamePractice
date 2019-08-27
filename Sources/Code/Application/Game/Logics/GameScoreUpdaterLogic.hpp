#ifndef __GAME_SCORE_UPDATER_LOGIC_HPP__
#define __GAME_SCORE_UPDATER_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameScoreUpdaterLogic : public EntityLogic,
    public ETNode<ETGameScoreUpdater> {
public:

    GameScoreUpdaterLogic();
    virtual ~GameScoreUpdaterLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameScoreUpdater
    void ET_setGameScore(int score) override;
};

#endif /* __GAME_SCORE_UPDATER_LOGIC_HPP__ */