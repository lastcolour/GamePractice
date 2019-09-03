#include "UITestUtils.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <gtest/gtest.h>

void CheckExpectedView(const char* expectedView) {
    EntityId activeViewId;
    ET_SendEventReturn(activeViewId, &ETUIViewStack::ET_getActiveViewId);

    ASSERT_NE(activeViewId, InvalidEntityId);
    std::string activeViewName;
    ET_SendEventReturn(activeViewName, activeViewId, &ETEntity::ET_getName);
    ASSERT_STREQ(expectedView, activeViewName.c_str());
}