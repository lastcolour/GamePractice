#include "UIUtils.hpp"
#include "Core/ETApplication.hpp"
#include "Config/UIConfig.hpp"
#include "UI/UILayoutStyle.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/ETEntity.hpp"

#include <cassert>

namespace UI {

UIBoxMargin CalculateMargin(const UIBoxStyle::Margin& margin, const Transform& tm) {
    UIBoxMargin resMargin;

    auto uiConfig = ET_getShared<UIConfig>();
    resMargin.left = uiConfig->getSizeOnGrind(margin.left);
    resMargin.right = uiConfig->getSizeOnGrind(margin.right);
    resMargin.bot = uiConfig->getSizeOnGrind(margin.bot);
    resMargin.top = uiConfig->getSizeOnGrind(margin.top);

    const auto& scale = tm.scale;
    resMargin.bot = static_cast<int>(resMargin.bot * scale.y);
    resMargin.top = static_cast<int>(resMargin.top * scale.y);
    resMargin.left = static_cast<int>(resMargin.left * scale.x);
    resMargin.right = static_cast<int>(resMargin.right * scale.x);

    return resMargin;
}

const char* GetViewTypeName(UIViewType viewType) {
    const char* viewTypeStr = "Invalid";
    switch(viewType) {
        case UIViewType::None: {
            viewTypeStr = "None";
            break;
        }
        case UIViewType::Main: {
            viewTypeStr = "Main";
            break;
        }
        case UIViewType::EndGame: {
            viewTypeStr = "EndGame";
            break;
        }
        case UIViewType::Game: {
            viewTypeStr = "Game";
            break;
        }
        case UIViewType::PauseGame: {
            viewTypeStr = "PauseGame";
            break;
        }
        case UIViewType::Background: {
            viewTypeStr = "Background";
            break;
        }
        case UIViewType::Loading: {
            viewTypeStr = "Loading";
            break;
        }
        default: {
            break;
        }
    }
    return viewTypeStr;
}

bool IsInsideBox(const Vec2i& pt, const AABB2Di& box) {
    return pt >= box.bot && pt <= box.top;
}

Vec2i CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, AABB2Di& parentBox, AABB2Di& box) {
    Vec2i center(0);
    switch(xAlign) {
        case UIXAlign::Left: {
            center.x = parentBox.bot.x + box.getSize().x / 2;
            break;
        }
        case UIXAlign::Center: {
            center.x = parentBox.getCenter().x;
            break;
        }
        case UIXAlign::Right: {
            center.x = parentBox.top.x - box.getSize().x / 2;
            break;
        }
        default: {
            assert(false && "Invalid x-aligment type");
        }
    }
    switch(yAlign) {
        case UIYAlign::Bot: {
            center.y = parentBox.bot.y + box.getSize().y / 2;
            break;
        }
        case UIYAlign::Center: {
            center.y = parentBox.getCenter().y;
            break;
        }
        case UIYAlign::Top: {
            center.y = parentBox.top.y - box.getSize().y / 2;
            break;
        }
        default: {
            assert(false && "Invalid y-aligment type");
        }
    }
    return center;
}

void Set2DPosition(EntityId elemId, const Vec2i& pos) {
    Transform tm;
    ET_SendEventReturn(tm, elemId, &ETEntity::ET_getTransform);

    tm.pt.x = static_cast<float>(pos.x);
    tm.pt.y = static_cast<float>(pos.y);

    ET_SendEvent(elemId, &ETEntity::ET_setTransform, tm);
}

} // namespace UI
