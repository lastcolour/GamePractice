cmake_minimum_required(VERSION 3.15)

project(Engine CXX)

include(dependicies.cmake)

add_subdirectory(Core)
add_subdirectory(Parallel)
add_subdirectory(Platform)
add_subdirectory(Math)
add_subdirectory(Reflect)
add_subdirectory(Entity)
add_subdirectory(Render)
add_subdirectory(Audio)
add_subdirectory(UI)

set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Engine include dir")

include_directories(
    ${CORE_INCLUDE_DIR}
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
target_link_libraries(${PROJECT_NAME}
    Core
    Parallel
    Math
    Reflect
    Platform
    Render
    Audio
    Entity
    UI
    Application)