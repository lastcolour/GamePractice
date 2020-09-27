#ifndef __UI_UTILS_HPPP__
#define __UI_UTILS_HPPP__

#include "UI/UIBoxStyle.hpp"
#include "UI/ETUIView.hpp"
#include "Math/Transform.hpp"
#include "Math/AABB.hpp"
#include "UI/UILayoutStyle.hpp"

class UILayoutStyle;

namespace UI {

UIBoxMargin CalculateMargin(const UIBoxStyle::Margin& margin, const Transform& tm);

const char* GetViewTypeName(UIViewType viewType);

bool IsInsideBox(const Vec2i& pt, const AABB2Di& box);

Vec2i CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, AABB2Di& parentBox, AABB2Di& box);

void Set2DPosition(EntityId elemId, const Vec2i& pos);

int GetValueOnGrind(float val);

float CovertValueFromGrid(int val);

} // namespace

#endif /* __UI_UTILS_HPPP__ */