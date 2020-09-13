#include "UIUtils.hpp"
#include "Core/ETApplication.hpp"
#include "UIConfig.hpp"

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

} // namespace UI
