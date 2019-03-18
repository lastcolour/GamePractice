#include "UI/UIStyle.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

XAlignType parseXAlignType(const std::string& strType) {
    if(strType == "center") {
        return XAlignType::Center;
    } else if(strType == "left") {
        return XAlignType::Left;
    } else if(strType == "right") {
        return XAlignType::Right;
    } else {
        LogWarning("[UIStyle::parseAlignType] Invalid x-align type: %s", strType);
    }
    return XAlignType::Center;
}

YAlignType parseYAlignType(const std::string& strType) {
    if(strType == "center") {
        return YAlignType::Center;
    } else if(strType == "top") {
        return YAlignType::Top;
    } else if(strType == "bot") {
        return YAlignType::Bot;
    } else {
        LogWarning("[UIStyle::parseAlignType] Invalid y-align type: %s", strType);
    }
    return YAlignType::Center;
}

SizeInvariant parseSizeInvariant(const std::string& strType) {
    if(strType == "rel") {
        return SizeInvariant::Relative;
    } else if(strType == "relBS") {
        return SizeInvariant::Relative;
    } else if(strType == "abs") {
        return SizeInvariant::Relative;
    } else if(strType == "absBS") {
        return SizeInvariant::Relative;
    } else if(strType == "pix") {
        return SizeInvariant::Relative;
    } else {
        LogWarning("[UIStyle::parseSizeInvariant] Invalid size invariant type: %s", strType);
    }
    return SizeInvariant::Relative;
}

} // namespace


Margin::Margin() :
    left(0.f),
    right(0.f),
    bot(0.f),
    top(0.f) {
}

UIStyle::UIStyle() :
    size(1.f),
    color(255, 255, 255),
    sizeInv(SizeInvariant::Relative),
    xAlignType(XAlignType::Center),
    yAlignType(YAlignType::Center) {
}

UIStyle::~UIStyle() {
}

bool UIStyle::serialize(const JSONNode& node) {
    if(auto alignNode = node.object("align")) {
        std::string alignTypeStr("center");
        node.value("x", alignTypeStr);
        xAlignType = parseXAlignType(alignTypeStr);

        alignTypeStr = "center";
        node.value("y", alignTypeStr);
        yAlignType = parseYAlignType(alignTypeStr);
    } else {
        LogWarning("[UIStyle::serialize] Can't find required node: 'align'");
    }
    if(auto sizeNode = node.object("size")) {
        sizeNode.value("w", size.x);
        sizeNode.value("h", size.y);
        if(size.x <= 0 || size.y <= 0) {
            LogWarning("[UIStyle::serialize] Invalid negative size: [%d, %d]", size.x, size.y);
            size = Vec2(1.f);
        }
        std::string sizeInvStr("rel");
        sizeNode.value("inv", sizeInvStr);
        sizeInv = parseSizeInvariant(sizeInvStr);
    } else {
        LogWarning("[UIStyle::serialize] Can't find required node: 'size'");
    }
    if(auto colorNode = node.object("color")) {
        int val = 0;
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
    if(auto marginNode = node.object("margin")) {
        marginNode.value("left", margin.left);
        marginNode.value("right", margin.right);
        marginNode.value("top", margin.bot);
        marginNode.value("bot", margin.top);
    } else {
        LogWarning("[UIStyle::serialize] Can't find required node: 'margin'");
    }
    return true;
}