#include "UI/Logics/UIAppearOnTop.hpp"

UIAppearOnTop::UIAppearOnTop() :
    isAppearing(true) {
}

UIAppearOnTop::~UIAppearOnTop() {
}

bool UIAppearOnTop::serialize(const JSONNode& node) {
    return true;
}

bool UIAppearOnTop::init() {
    ETNode<ETUIAppearAnimation>::connect(getEntityId());
    return true;
}

bool UIAppearOnTop::ET_isAppearing() const {
    return isAppearing;
}

void UIAppearOnTop::ET_setAppear(bool flag) {
    isAppearing = flag;
}

bool UIAppearOnTop::ET_animate(float duration) {
    return true;
}

bool UIAppearOnTop::ET_isNeedHideOldView() const {
    return true;
}

float UIAppearOnTop::ET_getDuration() const {
    return 1.f;
}
