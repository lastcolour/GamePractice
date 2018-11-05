cmake_minimum_required(VERSION 3.10)

set(MAIN_PROJECT_NAME "Game01" CACHE INTERNAL "Build target name of main project")
set(MAIN_PROJECT_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path to sources of main project")

project(${MAIN_PROJECT_NAME})

# Detect & verify project configuration

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed for this project")
else()
    message(STATUS "Binaries output directory: ${CMAKE_BINARY_DIR}")
endif()

if(APP_BUILD_PLATFORM)
    message(STATUS "Build platform: ${APP_BUILD_PLATFORM}")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else()
    message(FATAL_ERROR "Build platform variable APP_BUILD_PLATFORM not specified")
endif()

if(APP_BUILD_PLATFORM STREQUAL "Windows" OR APP_BUILD_PLATFORM STREQUAL "Linux")
    message(STATUS "Build unittests: ${BUILD_UNITTESTS}")
    set(BUILD_UNITTESTS TRUE CACHE INTERNAL "Build unittests")
    add_subdirectory(Tests)
else()
    message(STATUS "Skip unittests build (Not supported for such platform)")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/dependicies.cmake)

# Build target project

add_subdirectory(Application)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Application)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/desktop_main.cpp)
target_link_libraries(${PROJECT_NAME} Application ${APPLICATION_LIB_DEPENDECIES})
