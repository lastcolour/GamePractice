#ifndef __LEVELS_VIEW_SCRIPT_HPP__
#define __LEVELS_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/EventSequence.hpp"

class LevelsViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelsViewScript();
    virtual ~LevelsViewScript();

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewGetFocus() override;
    void ET_onViewLostFocus() override;

protected:

    void onEvent(const UIEvent& event) override;

private:

    EntityId progressBoxId;
    EntityId progressLabelId;
    EntityId scrollAreaId;
    EventSequence eventSeq;
};

#endif /* __LEVELS_VIEW_SCRIPT_HPP__ */