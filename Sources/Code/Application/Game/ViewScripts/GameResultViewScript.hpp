#ifndef __GAME_RESULT_VIEW_SCRIPT_HPP__
#define __GAME_RESULT_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Game/ViewScripts/EventSequence.hpp"

class ReflectContext;

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

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;

private:

    EventSequence eventSeq;
    ProgressionStars progressStars;
    EntityId continueButtonId;
    EntityId timeValueId;
    EntityId scoreValueId;
    EntityId levelNameId;
    EntityId emitterId;
};

#endif /* __GAME_RESULT_VIEW_SCRIPT_HPP__ */