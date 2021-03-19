#ifndef __GAME_VIEW_SCRIPT_HPP__
#define __GAME_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Game/ETGameScore.hpp"
#include "Game/ETGame.hpp"
#include "Audio/SoundEvent.hpp"
#include "Platform/ETSurface.hpp"
#include "Game/ViewScripts/EventSequence.hpp"

class GameViewScript : public BaseViewScript,
    public ETNode<ETGameObjectiveEvents>,
    public ETNode<ETGameStateEvents>,
    public ETNode<ETSurfaceEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameViewScript();
    virtual ~GameViewScript();

    // EntityLogic
    void init() override;

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewLostFocus() override;
    void ET_onViewGetFocus() override;

    // ETGameStateEvents
    void ET_onGameEnterState(EGameState state) override;
    void ET_onGameLeaveState(EGameState state) override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveProgress type) override;

    // ETSurfaceEvents
    void ET_onSurfaceCreated() override {}
    void ET_onSurfaceDestroyed() override {}
    void ET_onSurfaceHidden() override;
    void ET_onSurfaceShown() override {}
    void ET_onSurfaceLostFocus() override {}
    void ET_onSurfaceGainFocus() override {}
    void ET_onSurfaceResized(const Vec2i& size) override {}

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;

private:

    enum class State {
        None = 0,
        ShowingStartInfo,
        Game,
        ShowingEndInfo,
        WaitingPostGame,
        WaitingEnd
    };

private:

    void setScriptState(State newState);

private:

    ProgressionStars progressStars;
    SoundEvent getStarEvent;
    EntityId timeInfoBoxId;
    EntityId boardSpawnerId;
    EntityId startInfoId;
    EntityId endInfoId;
    float postGameTime;
    State scriptState;
    bool gameLeftPostGame;
    EventSequence eventSeq;
};

#endif /* __GAME_VIEW_SCRIPT_HPP__ */