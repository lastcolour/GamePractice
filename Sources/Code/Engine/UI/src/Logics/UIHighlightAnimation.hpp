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
    public ETNode<ETUIAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIHighlightAnimation();
    virtual ~UIHighlightAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIAnimation
    void ET_start() override;
    float ET_getDuration() const override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    Transform startTm;
    EntityId targetId;
    float duration;
    float scale;
    float currentDuration;
};

#endif /* __UI_HIGHLIGHT_ANIMATION_HPP__ */