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
    ET_setText(text.c_str());
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

void UILabel::ET_setText(const char* newText) {
    text = newText;
    ET_SendEvent(getRendererId(), &ETRenderTextLogic::ET_setText, text.c_str());
    auto rootEntId = getRootUIBox();
    if(rootEntId == InvalidEntityId) {
        ET_boxResize();
    } else {
        ET_SendEvent(rootEntId, &ETUIBox::ET_boxResize);
    }
}