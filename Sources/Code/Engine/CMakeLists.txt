cmake_minimum_required(VERSION 3.15)

project(Engine CXX)

include(dependicies.cmake)

set(CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core/include CACHE PATH "Core include dir")
set(MATH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Math/include CACHE PATH "Math include dir")
set(REFLECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Reflect/include CACHE PATH "Reflect include dir")
set(PARALLEL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Parallel/include CACHE PATH "Parallel include dir")
set(PLATFORM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Platform/include CACHE PATH "Platform include dir")
set(ENTITY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Entity/include CACHE PATH "Entity include dir")
set(RENDER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Render/include CACHE PATH "Render include dir")
set(UI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/UI/include CACHE PATH "UI include dir")
set(AUDIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Audio include dir")
set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Engine include dir")

add_subdirectory(UI)
add_subdirectory(Render)
add_subdirectory(Entity)
add_subdirectory(Audio)
add_subdirectory(Parallel)
add_subdirectory(Math)
add_subdirectory(Platform)
add_subdirectory(Reflect)
add_subdirectory(Core)

include_directories(
    ${CORE_INCLUDE_DIR}
    ${PROFILER_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR}
    ${APPLICATION_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${UI_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${PARALLEL_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Application
    Platform
    UI
    Entity
    Audio
    Render
    Parallel
    Reflect
    Math
    Core)