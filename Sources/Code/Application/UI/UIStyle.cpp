#include "UI/UIStyle.hpp"
#include "Core/JSONNode.hpp"

UIStyle::UIStyle() :
    listType(ListType::Vertical),
    alignType(AlignType::Center),
    size(1.f) {
}

UIStyle::~UIStyle() {
}

void UIStyle::serialize(const JSONNode& node) {
}
