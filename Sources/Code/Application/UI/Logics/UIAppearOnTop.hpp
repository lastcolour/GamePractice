#ifndef __UI_APPEAR_ON_TOP_HPP__
#define __UI_APPEAR_ON_TOP_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"

class UIAppearOnTop : public EntityLogic,
    public ETNode<ETUIAppearAnimation> {
public:

    UIAppearOnTop();
    virtual ~UIAppearOnTop();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

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