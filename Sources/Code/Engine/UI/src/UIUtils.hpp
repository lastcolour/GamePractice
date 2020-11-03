#ifndef __UI_UTILS_HPPP__
#define __UI_UTILS_HPPP__

#include "UI/UIBoxStyle.hpp"
#include "UI/ETUIView.hpp"
#include "Math/Transform.hpp"
#include "Math/AABB.hpp"
#include "UI/UILayoutStyle.hpp"

class UILayoutStyle;

namespace UI {

UIBoxMargin CalculateMargin(EntityId entityId, const UIBoxStyle::Margin& margin);

const char* GetViewTypeName(UIViewType viewType);

Vec2i CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, AABB2Di& parentBox, AABB2Di& box);

void Set2DPositionDoNotUpdateLayout(EntityId elemId, const Vec2i& pos);

void SetTmDoNotUpdateLayout(EntityId elemId, const Transform& tm);

int GetValueOnGrind(float val);

float CovertValueFromGrid(int val);

Vec2i CalculateBoxSize(const UIBoxStyle& style);

int GetZIndexForChild(EntityId entityId);

AABB2Di GetTmScaledBox(EntityId entityId, const AABB2Di& box);

AABB2Di SetTmCenterToBox(EntityId entityId, const AABB2Di& box);

} // namespace

#endif /* __UI_UTILS_HPPP__ */