#ifndef __GAME_RESULT_VIEW_SCRIPT_HPP__
#define __GAME_RESULT_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"

class ReflectContext;

class GameResultViewScript : public BaseViewScript {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameResultViewScript();
    virtual ~GameResultViewScript();

    // BaseViewScript
    void ET_onGetFocus() override;

protected:

    void onEvent(const UIEvent& event) override;

private:

    ProgressionStars progressStars;
};

#endif /* __GAME_RESULT_VIEW_SCRIPT_HPP__ */