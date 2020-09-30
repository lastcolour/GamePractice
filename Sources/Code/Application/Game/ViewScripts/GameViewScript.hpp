#ifndef __GAME_VIEW_SCRIPT_HPP__
#define __GAME_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"

class ReflectContext;

class GameViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameViewScript();
    virtual ~GameViewScript();

    // BaseViewScript
    void ET_onLostFocus() override;
    void ET_onGetFocus() override;

protected:

    void onEvent(UIEventType eventType) override;
};

#endif /* __GAME_VIEW_SCRIPT_HPP__ */