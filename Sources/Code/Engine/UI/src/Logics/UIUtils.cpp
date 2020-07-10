#include "Logics/UIUtils.hpp"
#include "Core/ETApplication.hpp"
#include "UIConfig.hpp"

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

} // namespace UI
