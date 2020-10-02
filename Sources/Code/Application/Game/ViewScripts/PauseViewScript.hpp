#ifndef __PAUSE_VIEW_SCRIPT_HPP__
#define __PAUSE_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"

class ReflectContext;

class PauseViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    PauseViewScript();
    virtual ~PauseViewScript();

protected:

    void onEvent(const UIEvent& event) override;
};

#endif /* __PAUSE_VIEW_SCRIPT_HPP__ */