#ifndef __UI_PRESS_ANIMATION_HPP__
#define __UI_PRESS_ANIMATION_HPP__

#include "UI/ETUIAnimation.hpp"
#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUITimer.hpp"
#include "Math/Transform.hpp"

class SoundEvent;
class ReflectContext;

class UIPressAnimation : public EntityLogic,
    public ETNode<ETUIAnimation>,
    public ETNode<ETUITimerEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIPressAnimation();
    virtual ~UIPressAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIAnimation
    void ET_start() override;
    void ET_startReverse() override;
    void ET_pause() override;
    void ET_resume() override;
    void ET_reverse() override;
    bool ET_isReversed() const override;
    float ET_getDuration() const override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    void setSoundEvent(const char* soundName);

private:

    Transform startTm;
    std::shared_ptr<SoundEvent> soundEvent;
    float inDuration;
    float outDuration;
    float minScale;
    float currDuration;
    bool isReversed;
};

#endif /* __UI_PRESS_ANIMATION_HPP__ */