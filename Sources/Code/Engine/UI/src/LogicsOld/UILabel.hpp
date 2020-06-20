#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "UI/Logics/UIBaseBox.hpp"

class UILabel : public UIBaseBox,
    public ETNode<ETUILabel> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILabel();
    virtual ~UILabel();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // UIBaseBox
    void ET_setStyle(const UIStyle& style) override;

    // ETUILabel
    void ET_setText(const char* newText) override;
    const char* ET_getText() const override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;

protected:

    Vec2i calculateBoxSize(const AABB2Di& parentBox) const override;
    EntityId getRenderId() const override;

protected:

    void createRenderer();
    void updateRendererParams() const;

private:

    std::string text;
    UILabelStyle labelStyle;
    EntityId renderId;
};

#endif /* __UI_LABEL_HPP__ */