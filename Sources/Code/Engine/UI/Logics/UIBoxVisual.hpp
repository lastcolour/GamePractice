#ifndef __UI_BOX_VISUAL_HPP__
#define __UI_BOX_VISUAL_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Render/Color.hpp"

class UIBoxVisual : public EntityLogic,
    public ETNode<ETUIBoxEvents>,
    public ETNode<ETUIVisibleElement> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBoxVisual();
    virtual ~UIBoxVisual();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIVisibleElement
    void ET_show() override;
    void ET_hide() override;
    bool ET_isVisible() const override;
    int ET_getZIndex() const override;
    void ET_setZIndex(int newZIndex) override;

    // ETUIBoxEvents
    void ET_onBoxResized() override;

private:

    void setRenderEntity(const char* renderName);

private:

    EntityId renderId;
    ColorB background;
};

#endif /* __UI_BOX_VISUAL_HPP__ */