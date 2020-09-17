#ifndef __UI_GAME_END_VIEW_SCRIPT_HPP__
#define __UI_GAME_END_VIEW_SCRIPT_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIGameEndView.hpp"

class ReflectContext;

class UIGameEndViewScript : public EntityLogic,
    public ETNode<ETUIViewAppearAnimationEvents>,
    public ETNode<ETUIStarsAppearAnimationEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIGameEndViewScript();
    virtual ~UIGameEndViewScript();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIViewAppearAnimationEvents
    void ET_onViewAppeared(EntityId viewId) override;
    void ET_onViewDisappeared(EntityId viewId) override {}

    // ETStarsAppearAnimationEvents
    void ET_onStarsAppeared() override;
};

#endif /* __UI_GAME_END_VIEW_SCRIPT_HPP__ */