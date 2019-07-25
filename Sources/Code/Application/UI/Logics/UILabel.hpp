#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "UI/Logics/UIBox.hpp"

class UILabel : public UIBox,
    public ETNode<ETUILabel> {
public:

    UILabel();
    virtual ~UILabel();

    // UIBox
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIBox
    void ET_addChildElement(EntityId childId) override {}

    // ETUILabel
    void ET_setText(const char* newText) override;
    const char* ET_getText() const override;

protected:

    Vec2i calculateBoxSize(const AABB2Di& parentBox) const override;

private:

    std::string text;
};

#endif /* __UI_LABEL_HPP__ */