#ifndef __UI_APPEAR_FROM_SIDE_HPP__
#define __UI_APPEAR_FROM_SIDE_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"

class UIAppearFromSide : public EntityLogic,
    public ETNode<ETUIAppearAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIAppearFromSide();
    virtual ~UIAppearFromSide();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIAppearAnimation
    void ET_setAppear(bool flag) override;
    bool ET_isAppearing() const override;
    bool ET_animate(float duration) override;
    bool ET_isNeedHideOldView() const override;
    float ET_getDuration() const override;

private:

    enum class AnimState {
        Init = 0,
        Animating,
        Done
    };

private:

    void initAnimation();
    void updateAnimation(float duration);
    void endAnimation();

private:

    float animDuration;
    float viewScale;
    AnimState animState;
    bool isAppearing;
};

#endif /* __UI_APPEAR_FROM_SIDE_HPP__ */