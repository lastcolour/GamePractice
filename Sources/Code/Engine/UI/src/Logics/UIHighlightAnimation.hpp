#ifndef __UI_HIGHLIGHT_ANIMATION_HPP__
#define __UI_HIGHLIGHT_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUITImer.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Math/Transform.hpp"

class ReflectContext;

class UIHighlightAnimation : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIHighlightAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIHighlightAnimation();
    virtual ~UIHighlightAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIHighlightAnimation
    void ET_playHightlight(EntityId triggerId) override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    float getTotalDuration() const;

private:

    Transform startTm;
    EntityId targetId;
    EntityId senderId;
    float duration;
    float scale;
    float currentDuration;
};

#endif /* __UI_HIGHLIGHT_ANIMATION_HPP__ */