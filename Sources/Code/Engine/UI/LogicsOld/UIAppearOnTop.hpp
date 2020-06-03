#ifndef __UI_APPEAR_ON_TOP_HPP__
#define __UI_APPEAR_ON_TOP_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"

class UIAppearOnTop : public EntityLogic,
    public ETNode<ETUIAppearAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIAppearOnTop();
    virtual ~UIAppearOnTop();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIAppearAnimation
    bool ET_isAppearing() const override;
    void ET_setAppear(bool flag) override;
    bool ET_animate(float duration) override;
    bool ET_isNeedHideOldView() const override;
    float ET_getDuration() const override;

private:

    bool isAppearing;
};

#endif /* __UI_APPEAR_ON_TOP_HPP__ */