#ifndef __UI_UTILS_HPPP__
#define __UI_UTILS_HPPP__

#include "UI/UIBoxStyle.hpp"
#include "Math/Transform.hpp"

namespace UI {

UIBoxMargin CalculateMargin(const UIBoxStyle::Margin& margin, const Transform& tm);

} // namespace

#endif /* __UI_UTILS_HPPP__ */