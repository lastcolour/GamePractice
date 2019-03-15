#include "UI/UIStyle.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

UIStyle::UIStyle() :
    size(1.f),
    color(255, 255, 255),
    sizeInv(SizeInvariant::Relative),
    alignType(AlignType::Center) {
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
    if(auto sizeNode = node.object("size")) {
        sizeNode.value("w", size.x);
        sizeNode.value("h", size.y);
        if(size.x <= 0 || size.y <= 0) {
            LogWarning("[UIStyle::serialize] Invalid size: [%d, %d]", size.x, size.y);
        }
        std::string sizeInvStr("rel");
        sizeNode.value("inv", sizeInvStr);
        if(sizeInvStr == "rel") {
            sizeInv == SizeInvariant::Relative;
        } else if(sizeInvStr == "relBS") {
            sizeInv == SizeInvariant::Relative;
        } else if(sizeInvStr == "abs") {
            sizeInv == SizeInvariant::Relative;
        } else if(sizeInvStr == "absBS") {
            sizeInv == SizeInvariant::Relative;
        } else if(sizeInvStr == "pix") {
            sizeInv == SizeInvariant::Relative;
        } else {
            LogWarning("[UIStyle::serialize] Invalid size invariant type: %s", sizeInvStr);
            return false;
        }
    } else {
        LogWarning("[UIStyle::serialize] Can't find required node: 'size'");
        return false;
    }
    if(auto colorNode = node.object("color")) {
        int val;
        colorNode.value("r", val);
        color.r = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
        val = 0;
        colorNode.value("g", val);
        color.g = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
        val = 0;
        colorNode.value("b", val);
        color.b = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
        val = 255;
        colorNode.value("a", val);
        color.a = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
    }
    return true;
}
