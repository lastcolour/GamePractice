#ifndef __UIBOX_HPP__
#define __UIBOX_HPP__

#include "Game/GameLogic.hpp"
#include "UI/UIETInterfaces.hpp"
#include "UI/UIStyle.hpp"

class UIBox : public GameLogic,
    public ETNode<ETUIBox> {
public:

    UIBox();
    virtual ~UIBox();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIBox
    const AABB2Di& ET_getAaabb2di() const override;
    void ET_setCenter(const Vec2i& center) override;
    void ET_alignInBox(const AABB2Di& alingBox) override;
    AABB2Di ET_getParentAaabb2di() const override;

protected:

    AABB2Di calcBox() const;
    Vec2i calcSize(const AABB2Di& parentBox) const;
    Vec2i calcCenter(const AABB2Di& selfBox, const AABB2Di& parentBox) const;
    AABB2Di caclParentBox() const;

    void setBox(const AABB2Di& newBox);
    const UIStyle& getStyle() const;
    void setStyle(const UIStyle& style);

private:

    UIStyle style;
    AABB2Di box;
};

#endif /* __UIBOX_HPP__ */