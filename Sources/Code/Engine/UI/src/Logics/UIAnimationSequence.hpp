#ifndef __UI_ANIMATION_SEQUENCE_HPP__
#define __UI_ANIMATION_SEQUENCE_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIAnimation.hpp"
#include "UI/ETUIBox.hpp"
#include "Logics/UIAnimationFrame.hpp"
#include "Logics/AdditiveUITransform.hpp"

class UIAnimationSequence : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIAnimationSequence>,
    public ETNode<ETUIElementEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIAnimationSequence();
    virtual ~UIAnimationSequence();

    // EntityLogic
    void init() override;
    void deinit() override;
    void onLoaded() override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

    // ETUIAnimationSequence
    void ET_playAnimation(EntityId animTriggerId) override;
    void ET_stopAnimation() override;
    void ET_addAnimationFrame(const UIAnimationFrame& newFrame) override;
    EAnimSequenceType ET_getType() const override;
    void ET_setType(EAnimSequenceType newType) override;
    void ET_setLooped(bool flag) override;
    void ET_addSubAnimation(EntityId subAnimId) override;
    void ET_setStartEndEvents(EShowEvent newStartEvent, EShowEvent newEndEvent) override;
    bool ET_isPlaying() const override;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2D& newAabb) override {}
    void ET_onZIndexChanged(int newZIndex) override {}
    void ET_onAlphaChanged(float newAlpha) override {}
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override {}

private:

    void restartCycle();
    void resetAnimState();
    void processFrame(float dt, UIAnimationFrame& task);
    bool isAnimHasShowEvent() const;
    bool canFinishAnim() const;

private:

    struct PrevState {
        Vec2 offset;
        Vec2 scale;
        float alpha;
    };

private:

    PrevState prevState;
    std::vector<EntityId> subAnimations;
    std::vector<UIAnimationFrame> frames;
    AddtiveUITransform currUIAddTm;
    EntityId triggerId;
    float currDuration;
    float startDelay;
    EAnimSequenceType seqType;
    EShowEvent onStartEvent;
    EShowEvent onEndEvent;
    bool cyclic;
    bool autoStart;
    bool isPlaying;
    bool disablePlaying;
    bool waitChildren;
    bool enableOnStop;
};

#endif /* __UI_ANIMATION_SEQUENCE_HPP__ */