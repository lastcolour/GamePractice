#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "UI/Logics/UIBaseBox.hpp"

class UILabel : public UIBaseBox,
    public ETNode<ETUILabel> {
public:

    UILabel();
    virtual ~UILabel();

    // UIBaseBox
    bool serialize(const JSONNode& node) override;
    bool init() override;
    void ET_setStyle(const UIStyle& style) override;

    // ETUILabel
    void ET_setText(const char* newText) override;
    const char* ET_getText() const override;

    // ETGameObjectEvents
    void ET_onTransformChanged(const Transform& newTm) override;

protected:

    Vec2i calculateBoxSize(const AABB2Di& parentBox) const override;

    void createRenderer();
    void updateRendererParams() const;

private:

    std::string text;
    EntityId renderId;
};

#endif /* __UI_LABEL_HPP__ */