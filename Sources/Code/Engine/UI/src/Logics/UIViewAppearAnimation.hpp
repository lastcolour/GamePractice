#ifndef __UI_VIEW_APPEAR_ANIMATION_HPP__
#define __UI_VIEW_APPEAR_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETTimer.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIView.hpp"
#include "Logics/UIViewAppearAnimationElement.hpp"

#include <vector>

class ReflectContext;

class UIViewAppearAnimation : public EntityLogic,
    public ETNode<ETAppTimerEvents>,
    public ETNode<ETUIViewAppearAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIViewAppearAnimation();
    virtual ~UIViewAppearAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETAppTimerEvents
    void ET_onAppTick(float dt) override;

    // ETUIViewAppearAnimation
    void ET_appear() override;
    void ET_disappear() override;

private:

    std::vector<UIViewAppearAnimationElement> elements;
};

#endif /* __UI_VIEW_APPEAR_ANIMATION_HPP__ */