#ifndef __GAME_SCORE_LOGIC_HPP__
#define __GAME_SCORE_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class ReflectContext;

class GameScoreLogic : public EntityLogic,
    public ETNode<ETGameScore>,
    public ETNode<ETGameBoardElemDestoryEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameScoreLogic();
    virtual ~GameScoreLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETGameBoardElemDestoryEvents
    void ET_onElemsDestroyed(int count) override;

    // ETGameScore
    int ET_getGameScore() const override;

private:

    int score;
};

#endif /* __GAME_SCORE_LOGIC_HPP__ */