#ifndef __UI_PRESS_ANIMATION_HPP__
#define __UI_PRESS_ANIMATION_HPP__

#include "UI/ETUIAnimation.hpp"
#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUITimer.hpp"
#include "Math/Transform.hpp"
#include "Audio/SoundEvent.hpp"

class ReflectContext;

class UIPressAnimation : public EntityLogic,
    public ETNode<ETUIPressAnimation>,
    public ETNode<ETUITimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIPressAnimation();
    virtual ~UIPressAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIPressAnimation
    void ET_playPress(EntityId triggerId) override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    void setSoundEvent(const char* soundName);
    float getTotalDuration() const;

private:

    Transform startTm;
    SoundEvent soundEvent;
    EntityId senderId;
    float duration;
    float minScale;
    float currDuration;
};

#endif /* __UI_PRESS_ANIMATION_HPP__ */