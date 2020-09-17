#ifndef __UI_GAME_END_STARS_APPEAR_ANIMATION_HPP__
#define __UI_GAME_END_STARS_APPEAR_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIGameEndView.hpp"
#include "Math/Transform.hpp"

class ReflectContext;

class UIGameEndStarsAppearAnimation : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIStarsAppearAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIGameEndStarsAppearAnimation();
    virtual ~UIGameEndStarsAppearAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

    // ETUIStarsAppearAnimation
    void ET_showStars(int starsCount) override;

private:

    Transform starStartTm;
    EntityId firstId;
    EntityId secondId;
    EntityId thirdId;
    float duration;
    float startScale;
    float currentDuration;
    int currentStage;
    int endStage;
};

#endif /* __UI_GAME_END_STARS_APPEAR_ANIMATION_HPP__ */