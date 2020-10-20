#ifndef __GAME_VIEW_SCRIPT_HPP__
#define __GAME_VIEW_SCRIPT_HPP__

#include "Game/ViewScripts/BaseViewScript.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Game/ETGameScore.hpp"

class ReflectContext;

class GameViewScript : public BaseViewScript,
    public ETNode<ETGameObjectiveEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameViewScript();
    virtual ~GameViewScript();

    // EntityLogic
    bool init() override;

    // BaseViewScript
    void ET_onLostFocus() override;
    void ET_onGetFocus() override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveProgress type) override;

protected:

    void onEvent(const UIEvent& event) override;

private:

    ProgressionStars progressStars;
};

#endif /* __GAME_VIEW_SCRIPT_HPP__ */