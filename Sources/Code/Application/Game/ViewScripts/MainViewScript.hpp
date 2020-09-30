#ifndef __MAIN_VIEW_SCRIPT_HPP__
#define __MAIN_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"

class ReflectContext;

class MainViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    MainViewScript();
    virtual ~MainViewScript();

protected:

    void onEvent(UIEventType eventType) override;
};

#endif /* __MAIN_VIEW_SCRIPT_HPP__ */