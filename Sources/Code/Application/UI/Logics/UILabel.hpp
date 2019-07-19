#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include "UI/Logics/UIBox.hpp"

class UILabel : public UIBox,
    public ETNode<ETUILabel> {
public:

    UILabel();
    virtual ~UILabel();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIBox
    void ET_addChildElement(EntityId childId) {}

    // ETUILabel
    void ET_setText(const std::string& text) override;

private:


    std::string text;
};

#endif /* __UI_LABEL_HPP__ */