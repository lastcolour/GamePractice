#ifndef __UI_UTILS_HPPP__
#define __UI_UTILS_HPPP__

#include "UI/UIBoxStyle.hpp"
#include "UI/ETUIView.hpp"
#include "Math/Transform.hpp"
#include "Math/AABB.hpp"
#include "UI/UILayoutStyle.hpp"

class UILayoutStyle;
class UIAnimationSequence;
enum class EAnimSequenceType;

namespace UI {

UIBoxMargin CalculateMargin(EntityId entityId, const UIBoxStyle::Margin& margin);

const char* GetViewTypeName(UIViewType viewType);

Vec2i CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, const AABB2Di& parentBox, const AABB2Di& box);

void Set2DPositionDoNotUpdateLayout(EntityId elemId, const Vec2i& pos);

void SetTMDoNotUpdateLayout(EntityId elemId, const Transform& tm);

void SetLocalTMDoNotUpdateLayout(EntityId elemId, const Transform& tm);

float GetValueOnGrind(float val);

float ConvertValueFromGrid(float val);

Vec2i CalculateBoxSize(const UIBoxStyle& style);

int GetZIndexForChild(EntityId entityId);

AABB2Di SetTmCenterToBox(EntityId entityId, const AABB2Di& box);

AABB2Di ApplyEntityTmToBox(EntityId entityId, const AABB2Di& box);

bool IsRootViewHasFocus(EntityId elemId);

UIAnimationSequence* GetAnimation(EAnimSequenceType animType, EntityId entityId);

void CopyUIElemAttribsFromParent(EntityId fromId, EntityId toId);

} // namespace UI

#endif /* __UI_UTILS_HPPP__ */