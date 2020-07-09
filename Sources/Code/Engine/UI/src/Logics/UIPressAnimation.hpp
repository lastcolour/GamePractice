#ifndef __UI_PRESS_ANIMATION_HPP__
#define __UI_PRESS_ANIMATION_HPP__

#include "UI/ETUIAnimation.hpp"
#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETTimer.hpp"
#include "Math/Transform.hpp"

struct Sound;
class ReflectContext;

class UIPressAnimation : public EntityLogic,
    public ETNode<ETUIAnimation>,
    public ETNode<ETAppTimerEvents> {
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

    // ETAppTimerEvents
    void ET_onAppTick(float dt) override;

private:

    void setSound(const char* soundName);

private:

    Transform startTm;
    std::unique_ptr<Sound> sound;
    float inDuration;
    float outDuration;
    float minScale;
    float currDuration;
    bool isReversed;
};

#endif /* __UI_PRESS_ANIMATION_HPP__ */