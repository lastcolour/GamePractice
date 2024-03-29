cmake_minimum_required(VERSION 3.16)

project(Application CXX)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${UI_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR})

file(GLOB APPLICATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/Logics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/States/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/ViewScripts/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/Progression/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Laucher/*.cpp)

file(GLOB APPLICATION_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/Logics/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/States/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/ViewScripts/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/Progression/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Laucher/*.hpp)

add_library(${PROJECT_NAME} STATIC ${APPLICATION_SOURCES} ${APPLICATION_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE Core)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    [["Core/Core.hpp"]]
    [["Entity/ETEntity.hpp"]]
)