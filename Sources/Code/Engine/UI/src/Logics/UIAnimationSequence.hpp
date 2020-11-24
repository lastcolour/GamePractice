#ifndef __UI_ANIMATION_SEQUENCE_HPP__
#define __UI_ANIMATION_SEQUENCE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUITimer.hpp"
#include "Logics/UIAnimationFrame.hpp"
#include "UI/ETUIAnimation.hpp"

#include <vector>

class ReflectContext;

class UIAnimationSequence : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIAnimationSequence> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIAnimationSequence();
    virtual ~UIAnimationSequence();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

    // ETUIAnimationSequence
    bool ET_playAnimation(EntityId animTriggerId, EAnimSequenceType filter) override;
    void ET_stopAnimation(EAnimSequenceType filter) override;
    void ET_addAnimationFrame(const UIAnimationFrame& newFrame) override;
    EAnimSequenceType ET_getType() const override;
    void ET_setType(EAnimSequenceType newType) override;
    void ET_setLooped(bool flag) override;
    void ET_addSubAnimation(EntityId subAnimId) override;

private:

    void startCycle();
    void processFrame(float dt, UIAnimationFrame& task);

private:

    struct PrevState {
        Transform tm;
        float alpha;
    };

private:

    PrevState prevState;
    std::vector<EntityId> subAnimations;
    std::vector<UIAnimationFrame> frames;
    Transform origTm;
    float origAlpha;
    float currDuration;
    float startDelay;
    EntityId triggerId;
    EAnimSequenceType seqType;
    bool cyclic;
    bool autoStart;
    bool isPlaying;
};

#endif /* __UI_ANIMATION_SEQUENCE_HPP__ */