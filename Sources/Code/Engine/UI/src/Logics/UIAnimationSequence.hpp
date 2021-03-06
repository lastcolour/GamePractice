#ifndef __UI_ANIMATION_SEQUENCE_HPP__
#define __UI_ANIMATION_SEQUENCE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Entity/ETEntity.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Logics/UIAnimationFrame.hpp"

#include <vector>

class ReflectContext;

class UIAnimationSequence : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIAnimationSequence>,
    public ETNode<ETEntityEvents>,
    public ETNode<ETUIElementEvents>,
    public ETNode<ETUIAdditiveAnimationTarget> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIAnimationSequence();
    virtual ~UIAnimationSequence();

    // EntityLogic
    void init() override;
    void deinit() override;

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

    // ETUIAdditiveAnimationTarget
    void ET_applyAdditiveTranform() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override;

private:

    void reStartCycle();
    void processFrame(float dt, UIAnimationFrame& task);
    bool isAnimHasShowEvent() const;

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
    Transform currAddTm;
    EntityId triggerId;
    float currAddAlpha;
    float currDuration;
    float startDelay;
    EAnimSequenceType seqType;
    EShowEvent onStartEvent;
    EShowEvent onEndEvent;
    bool cyclic;
    bool autoStart;
    bool isPlaying;
};

#endif /* __UI_ANIMATION_SEQUENCE_HPP__ */