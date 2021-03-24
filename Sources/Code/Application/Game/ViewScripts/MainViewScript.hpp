#ifndef __MAIN_VIEW_SCRIPT_HPP__
#define __MAIN_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"

class MainViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    MainViewScript();
    virtual ~MainViewScript();

    // BaseViewScript
    void ET_onViewGetFocus() override;

protected:

    // BaseViewScript
    void onEvent(const UIEvent& event) override;
};

#endif /* __MAIN_VIEW_SCRIPT_HPP__ */