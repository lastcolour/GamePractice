#ifndef __UI_BUTTON_HPP__
#define __UI_BUTTON_HPP__

#include "Logics/UIBox.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIViewScript.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Core/TimePoint.hpp"
#include "Audio/SoundEvent.hpp"

class UIButton : public UIBox,
    public ETNode<ETUIInteractionBox>,
    public ETNode<ETUIAnimationSequenceEvent> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIButton();
    virtual ~UIButton();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIInteractionBox
    EInputEventResult ET_onInputEvent(EActionType type, const Vec2i& pt) override;

    // ETUIAnimationSequenceEvent
    void ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) override;

    // UIBox
    void ET_onLoaded() override;

protected:

    void onDisabled(bool flag) override;

private:

    EInputEventResult onPress(const Vec2i& pt);
    EInputEventResult onMove(const Vec2i& pt);
    EInputEventResult onRelease(const Vec2i& pt);
    bool canContinueEvent(const Vec2i& pt) const;

private:

    Vec2i pressPt;
    TimePoint pressTime;
    UIEvent::EventType eventType;
    SoundEvent pressSound;
    bool isLoaded;
};

#endif /* __UI_BUTTON_HPP__ */