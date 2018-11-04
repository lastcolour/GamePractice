cmake_minimum_required(VERSION 3.10)

project(Game01)

set(TESTS_PROJECT_NAME ${PROJECT_NAME}Tests)

if(APP_BUILD_PLATFORM)
    message(STATUS "Build platform: ${APP_BUILD_PLATFORM}")
else()
    message(FATAL_ERROR "Build platform variable APP_BUILD_PLATFORM not specified")
endif()

add_subdirectory(Application)
add_subdirectory(Tests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Application)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/desktop_main.cpp)
target_link_libraries(${PROJECT_NAME} Application)

add_executable(${TESTS_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests_main.cpp)
target_link_libraries(${TESTS_PROJECT_NAME} Application)