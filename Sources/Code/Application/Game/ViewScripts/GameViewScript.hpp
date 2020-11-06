#ifndef __GAME_VIEW_SCRIPT_HPP__
#define __GAME_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Game/ETGameScore.hpp"
#include "Game/ETGame.hpp"
#include "Audio/SoundEvent.hpp"

class ReflectContext;

class GameViewScript : public BaseViewScript,
    public ETNode<ETGameObjectiveEvents>,
    public ETNode<ETGameStateEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameViewScript();
    virtual ~GameViewScript();

    // EntityLogic
    bool init() override;

    // BaseViewScript
    void ET_onViewLostFocus() override;
    void ET_onViewGetFocus() override;

    // ETGameStateEvents
    void ET_onGameEnterState(EGameState state) override;
    void ET_onGameLeaveState(EGameState state) override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveProgress type) override;

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;

private:

    void setGetStatSoundEvent(const char* eventName);

private:

    ProgressionStars progressStars;
    SoundEvent getStarEvent;
    EntityId timeInfoBoxId;
    EntityId boardSpawnerId;
};

#endif /* __GAME_VIEW_SCRIPT_HPP__ */