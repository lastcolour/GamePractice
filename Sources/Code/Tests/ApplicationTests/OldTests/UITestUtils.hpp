#ifndef __UI_TEST_UTILS_HPP__
#define __UI_TEST_UTILS_HPP__

#include <Math/Vector.hpp>

class UIBaseBox;

void CheckExpectedView(const char* expectedView);

void WaitViewSwitchEnd();

void WaitButtonReleaseEvent();

void CheckUIBoxSizeAndCenter(const UIBaseBox* uiBox, const Vec2i& expectedSize, const Vec2i& expectedCenter);

#endif /* __UI_TEST_UTILS_HPP__ */