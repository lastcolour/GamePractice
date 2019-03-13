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

protected:

    UIStyle style;
    AABB2Di box;

private:

    AABB2Di calcBox() const;
};

#endif /* __UIBOX_HPP__ */