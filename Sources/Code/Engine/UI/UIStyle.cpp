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
        return SizeInvariant::RelativeBiggestSquare;
    } else if(strType == "abs") {
        return SizeInvariant::Absolute;
    } else if(strType == "absBS") {
        return SizeInvariant::AbsoluteBiggestSquare;
    } else if(strType == "pix") {
        return SizeInvariant::Pixel;
    } else {
        LogWarning("[UIStyle::parseSizeInvariant] Invalid size invariant type: %s", strType);
    }
    return SizeInvariant::Relative;
}

ColorB parseColor(const JSONNode& node) {
    ColorB color;
    int val = 0;
    node.read("r", val);
    color.r = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
    val = 0;
    node.read("g", val);
    color.g = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
    val = 0;
    node.read("b", val);
    color.b = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
    val = 255;
    node.read("a", val);
    color.a = static_cast<uint8_t>(Math::Clamp(val, 0, 255));
    return color;
}

} // namespace

BoxMargin::BoxMargin() :
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
    yAlignType(YAlignType::Center),
    margin(),
    renderer(),
    fontSize(0.5f),
    fontColor(0, 0, 0) {
}

UIStyle::~UIStyle() {
}

void UIStyle::serialize(const JSONNode& node) {
    if(auto alignNode = node.object("align")) {
        std::string alignTypeStr("center");
        alignNode.read("x", alignTypeStr);
        xAlignType = parseXAlignType(alignTypeStr);

        alignTypeStr = "center";
        alignNode.read("y", alignTypeStr);
        yAlignType = parseYAlignType(alignTypeStr);
    }
    if(auto sizeNode = node.object("size")) {
        sizeNode.read("w", size.x);
        sizeNode.read("h", size.y);
        if(size.x <= 0 || size.y <= 0) {
            LogWarning("[UIStyle::serialize] Invalid negative size: [%d, %d]", size.x, size.y);
            size = Vec2(1.f);
        }
        std::string sizeInvStr("rel");
        sizeNode.read("inv", sizeInvStr);
        sizeInv = parseSizeInvariant(sizeInvStr);
    }
    if(auto colorNode = node.object("color")) {
        color = parseColor(colorNode);
    } else {
        color = ColorB(0, 0, 0, 0);
    }
    if(auto fontColorNode = node.object("fontColor")) {
        fontColor = parseColor(fontColorNode);
    } else {
        fontColor = ColorB(0, 0, 0, 0);
    }
    node.read("fontSize", fontSize);
    if(auto marginNode = node.object("margin")) {
        marginNode.read("left", margin.left);
        marginNode.read("right", margin.right);
        marginNode.read("top", margin.bot);
        marginNode.read("bot", margin.top);
    }
    node.read("renderer", renderer);
}