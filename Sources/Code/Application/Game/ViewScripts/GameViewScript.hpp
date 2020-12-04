#ifndef __GAME_VIEW_SCRIPT_HPP__
#define __GAME_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Game/ETGameScore.hpp"
#include "Game/ETGame.hpp"
#include "UI/ETUITimer.hpp"
#include "Audio/SoundEvent.hpp"
#include "UI/ETUIAnimation.hpp"

class ReflectContext;

class GameViewScript : public BaseViewScript,
    public ETNode<ETGameObjectiveEvents>,
    public ETNode<ETGameStateEvents>,
    public ETNode<ETUIAnimationSequenceEvent>,
    public ETNode<ETUITimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameViewScript();
    virtual ~GameViewScript();

    // EntityLogic
    bool init() override;

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewLostFocus() override;
    void ET_onViewGetFocus() override;

    // ETGameStateEvents
    void ET_onGameEnterState(EGameState state) override;
    void ET_onGameLeaveState(EGameState state) override;

    // ETUIAnimationSequenceEvent
    void ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveProgress type) override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;

private:

    void setGetStatSoundEvent(const char* eventName);

private:

    enum class State {
        None = 0,
        ShowStartInfo,
        Game,
        ShowEndInfo,
        WaitPostGame
    };

private:

    ProgressionStars progressStars;
    SoundEvent getStarEvent;
    EntityId timeInfoBoxId;
    EntityId boardSpawnerId;
    EntityId startInfoId;
    EntityId endInfoId;
    float postGameTime;
    float currentPostGameTime;
    State scriptState;
    bool gameLeftPostGame;
};

#endif /* __GAME_VIEW_SCRIPT_HPP__ */