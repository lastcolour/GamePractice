#ifndef __UI_BOX_HPP__
#define __UI_BOX_HPP__

#include "UI/Logics/UIBaseBox.hpp"

class UIBox : public UIBaseBox {
public:
    UIBox();
    virtual ~UIBox();

    // UIBaseBox
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIBox
    void ET_setStyle(const UIStyle& newStyle) override;
    void ET_boxResize() override;

protected:

    EntityId getRendererId() const;

private:

    void createRenderer();
    void updateRenderParams();
    void updateRenderSize();

private:

    EntityId renderId;
    EntityId labelId;
};

#endif /* __UI_BOX_HPP__ */