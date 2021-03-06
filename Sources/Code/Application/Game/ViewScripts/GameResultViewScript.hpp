#ifndef __GAME_RESULT_VIEW_SCRIPT_HPP__
#define __GAME_RESULT_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "UI/ETUIAnimation.hpp"

class ReflectContext;

class GameResultViewScript : public BaseViewScript,
    public ETNode<ETUIAnimationSequenceEvent> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameResultViewScript();
    virtual ~GameResultViewScript();

    // BaseViewScript
    void init() override;

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewGetFocus() override;

    // ETUIAnimationSequenceEvent
    void ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) override;

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