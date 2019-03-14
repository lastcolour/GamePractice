#include "UI/UIStyle.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

UIStyle::UIStyle() :
    alignType(AlignType::Center),
    size(1.f) {
}

UIStyle::~UIStyle() {
}

bool UIStyle::serialize(const JSONNode& node) {
    std::string align("center");
    node.value("align", align);
    if(align == "center") {
        alignType = AlignType::Center;
    } else {
        LogWarning("[UIStyle::serialize] Unknown aling type: %s", align);
        return false;
    }
    node.value("size", size);
    if(size.x <= 0 || size.y <= 0) {
        LogWarning("[UIStyle::serialize] Invalid size: [%d, %d]", size.x, size.y);
        return false;
    }
    return true;
}
