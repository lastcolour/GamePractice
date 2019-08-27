#ifndef __GAME_SCORE_LOGIC_HPP__
#define __GAME_SCORE_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameScoreLogic : public EntityLogic,
    public ETNode<ETGameScore> {
public:

    GameScoreLogic();
    virtual ~GameScoreLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameScore
    void ET_onElemsDestroyed(int count) override;

private:

    int score;
};

#endif /* __GAME_SCORE_LOGIC_HPP__ */