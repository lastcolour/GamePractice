#ifndef __UI_VIEW_APPEAR_ANIMATION_HPP__
#define __UI_VIEW_APPEAR_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUITimer.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Logics/UIViewAppearAnimationElement.hpp"

#include <vector>

class ReflectContext;

class UIViewAppearAnimation : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIViewAppearAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIViewAppearAnimation();
    virtual ~UIViewAppearAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

    // ETUIViewAppearAnimation
    void ET_playAppear(EntityId triggerId) override;
    void ET_playDissapear(EntityId triggerId) override;

private:

    void updateTransform(UIViewAppearAnimationElement& elem, float prog);
    void updateAlpha(UIViewAppearAnimationElement& elem, float prog);

private:

    enum class State {
        None = 0,
        Appear,
        Disappear
    };

private:

    std::vector<UIViewAppearAnimationElement> elements;
    float animDuration;
    State state;
    EntityId triggerEntId;
};

#endif /* __UI_VIEW_APPEAR_ANIMATION_HPP__ */