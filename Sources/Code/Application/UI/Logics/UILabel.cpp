#include "UI/Logics/UILabel.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

UILabel::UILabel() {
}

UILabel::~UILabel() {
}

bool UILabel::serialize(const JSONNode& node) {
    if(!UIBox::serialize(node)) {
        LogWarning("[UILabel::serialize] Can't serialize UIbox");
        return false;
    }
    if(getRendererId() == InvalidEntityId) {
        LogWarning("[UILabel::serialize] Can't serialize UIBox without renderer");
        return false;
    }
    node.value("text", text);
    return true;
}

bool UILabel::init() {
    if(!UIBox::init()) {
        LogWarning("[UILabel::init] Can't init UIbox");
        return false;
    }
    if(getRendererId() == InvalidEntityId) {
        LogWarning("[UILabel::init] Can't init UILabel without renderer");
        return false;
    }
    ETNode<ETUILabel>::connect(getEntityId());
    return true;
}

const char* UILabel::ET_getText() const {
    return text.c_str();
}

Vec2i UILabel::calculateBoxSize(const AABB2Di& parentBox) const {
    AABB2D renderTextBox(0.f);
    ET_SendEventReturn(renderTextBox, getRendererId(), &ETRenderTextLogic::ET_getTextAABB);
    auto renderBoxSize = renderTextBox.getSize();
    Vec2i size(static_cast<int>(renderBoxSize.x), static_cast<int>(renderBoxSize.y));
    return size;
}

void UILabel::ET_setText(const char* text) {
    const auto& style = ET_getStyle();
    ET_SendEvent(getRendererId(), &ETRenderTextLogic::ET_setText, text);
    ET_SendEvent(getRendererId(), &ETRenderTextLogic::ET_setColor, style.color);
    //ET_SendEvent(&ETRenderTextLogic::ET_setFontSize, style.fontSize);
}

void UILabel::ET_setStyle(const UIStyle& newStyle) {
}