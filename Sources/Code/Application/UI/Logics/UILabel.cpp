#include "UI/Logics/UILabel.hpp"
#include "ETApplicationInterfaces.hpp"

UILabel::UILabel() {
}

UILabel::~UILabel() {
}

bool UILabel::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UILabel::serialize] Can't serialize UIbox");
        return false;
    }
    return true;
}

bool UILabel::init() {
    if(!UIBox::init()) {
        LogWarning("[UILabel::serialize] Can't init UIbox");
        return false;
    }
    ETNode<ETUILabel>::connect(getEntityId());
    return true;
}

void UILabel::ET_setText(const std::string& text) {
    ET_SendEvent(&ETRenderTextLogic::ET_setText, text);
    //ET_SendEvent(&ETRenderTextLogic::ET_setColor, style.color);
    //ET_SendEvent(&ETRenderTextLogic::ET_setFontSize, style.fontSize);
}