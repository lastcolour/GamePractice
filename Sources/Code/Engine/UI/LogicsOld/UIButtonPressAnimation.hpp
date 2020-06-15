#ifndef __UI_BUTTON_PRESS_ANIMATION_HPP__
#define __UI_BUTTON_PRESS_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Core/ETTimer.hpp"

class UIButtonPressAnimation : public EntityLogic,
    public ETNode<ETAppTimerEvents>,
    public ETNode<ETUIButtonPressAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIButtonPressAnimation();
    virtual ~UIButtonPressAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETAppTimerEvents
    void ET_onAppTick(float dt) override;

    // ETUIButtonPressAnimation
    void ET_startPressAnimation() override;
    float ET_getAnimationDuration() const override;

private:

    Vec3 startScale;
    float inDuration;
    float outDuration;
    float minScale;
    float progress;
};

#endif /* __UI_BUTTON_PRESS_ANIMATION_HPP__ */