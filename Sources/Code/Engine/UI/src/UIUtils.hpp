#ifndef __UI_UTILS_HPPP__
#define __UI_UTILS_HPPP__

#include "UI/UIBoxStyle.hpp"
#include "UI/ETUIView.hpp"
#include "Math/AABB.hpp"
#include "UI/UILayoutStyle.hpp"

class UILayoutStyle;
class UIAnimationSequence;
enum class EAnimSequenceType;

namespace UI {

UIBoxMargin CalculateMargin(const Transform& tm, const UIBoxStyle::Margin& margin);

const char* GetViewTypeName(UIViewType viewType);

Vec2 CalcAligmentCenter(UIXAlign xAlign, UIYAlign yAlign, const AABB2D& parentBox, const AABB2D& box);

void Set2DPos(EntityId elemId, const Vec2& pos);

float GetValueOnGrind(float val);

float ConvertValueFromGrid(float val);

Vec2 CalculateBoxSize(const UIBoxStyle& style);

AABB2D ApplyTmToBox(const Transform& tm, const AABB2D& box);

bool IsRootViewHasFocus(EntityId elemId);

UIAnimationSequence* GetAnimation(EAnimSequenceType animType, EntityId entityId);

void CopyUIElemAttribsFromParent(EntityId fromId, EntityId toId);

} // namespace UI

#endif /* __UI_UTILS_HPPP__ */