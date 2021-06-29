#ifndef __GAME_RESULT_VIEW_SCRIPT_HPP__
#define __GAME_RESULT_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Game/ViewScripts/EventSequence.hpp"
#include "Audio/SoundEvent.hpp"

class GameResultViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameResultViewScript();
    virtual ~GameResultViewScript();

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewGetFocus() override;
    void ET_onViewLostFocus() override;

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;

private:

    SoundEvent starAppearSound;
    EventSequence eventSeq;
    ProgressionStars progressStars;
    EntityId continueButtonId;
    EntityId timeValueId;
    EntityId timeBoxId;
    EntityId scoreValueId;
    EntityId scoreBoxId;
    EntityId levelNameId;
    EntityId emitterId;
    EntityId shakeBoxId;
};

#endif /* __GAME_RESULT_VIEW_SCRIPT_HPP__ */