#ifndef __PAUSE_VIEW_SCRIPT_HPP__
#define __PAUSE_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"

class PauseViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    PauseViewScript();
    virtual ~PauseViewScript();

    // BaseViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;
};

#endif /* __PAUSE_VIEW_SCRIPT_HPP__ */