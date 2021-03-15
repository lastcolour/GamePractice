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

Vec2 CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, const AABB2D& parentBox, const AABB2D& box);

void Set2DPositionDoNotUpdateLayout(EntityId elemId, const Vec2& pos);

void SetTMDoNotUpdateLayout(EntityId elemId, const Transform& tm);

void SetLocalTMDoNotUpdateLayout(EntityId elemId, const Transform& tm);

float GetValueOnGrind(float val);

float ConvertValueFromGrid(float val);

Vec2 CalculateBoxSize(const UIBoxStyle& style);

int GetZIndexForChild(EntityId entityId);

AABB2D SetTmCenterToBox(EntityId entityId, const AABB2D& box);

AABB2D ApplyEntityTmToBox(EntityId entityId, const AABB2D& box);

bool IsRootViewHasFocus(EntityId elemId);

UIAnimationSequence* GetAnimation(EAnimSequenceType animType, EntityId entityId);

void CopyUIElemAttribsFromParent(EntityId fromId, EntityId toId);

} // namespace UI

#endif /* __UI_UTILS_HPPP__ */