#ifndef __GAME_RESULT_VIEW_SCRIPT_HPP__
#define __GAME_RESULT_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "UI/ETUIAnimation.hpp"

class ReflectContext;

class GameResultViewScript : public BaseViewScript,
    public ETNode<ETUIViewAppearAnimationEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameResultViewScript();
    virtual ~GameResultViewScript();

    // BaseViewScript
    bool init() override;

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewGetFocus() override;

    // ETUIViewAppearAnimationEvents
    void ET_onAppearPlayed(EntityId viewId) override;
    void ET_onDisappearPlayed(EntityId viewId) override;

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;

private:

    void playAppearAnimation(EntityId elemId);

private:

    enum class State {
        None,
        ShowingStars,
        ShowingContinueButton,
        Waiting
    };

private:

    State state;
    EntityId waitingId;
    ProgressionStars progressStars;
    EntityId continueButtonId;
    EntityId timeValueId;
    EntityId scoreValueId;
    EntityId levelNameId;
    EntityId emitterId;
};

#endif /* __GAME_RESULT_VIEW_SCRIPT_HPP__ */