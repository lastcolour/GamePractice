cmake_minimum_required(VERSION 3.10)

set(MAIN_PROJECT_NAME "Game01" CACHE INTERNAL "Build target name of main project")
set(MAIN_PROJECT_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path to sources of main project")

project(${MAIN_PROJECT_NAME} CXX)

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
    add_definitions(-DDESKTOP_APP_BUILD)
    if(APP_BUILD_PLATFORM STREQUAL "Windows")
        add_definitions(-DAPP_BUILD_PLATFORM_WINDOWS)
    else()
        add_definitions(-DAPP_BUILD_PLATFORM_LINUX)
    endif()
    set(BUILD_UNITTESTS TRUE CACHE INTERNAL "Build unittests")
else()
    message(STATUS "Skip unittests build (Not supported for such platform)")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/compile_flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/dependicies.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/assets_dir.cmake)

# Build target project

add_subdirectory(Application)
if(BUILD_UNITTESTS)
    add_subdirectory(Tests)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Application)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/desktop_main.cpp)
target_link_libraries(${PROJECT_NAME} Application ${APP_LIB_DEPENDICIES})
