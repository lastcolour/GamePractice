#ifndef __LEVELS_VIEW_SCRIPT_HPP__
#define __LEVELS_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"

class ReflectContext;

class LevelsViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelsViewScript();
    virtual ~LevelsViewScript();

    // BaseViewScript
    void ET_onViewOpened() override;

protected:

    void onEvent(const UIEvent& event) override;

private:

    EntityId progressLabelId;
};

#endif /* __LEVELS_VIEW_SCRIPT_HPP__ */