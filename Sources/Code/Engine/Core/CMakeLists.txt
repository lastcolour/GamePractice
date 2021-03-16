cmake_minimum_required(VERSION 3.16)

project(Core CXX)

include_directories(
    ${CORE_INCLUDE_DIR}
    ${RAPID_JSON_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${PARALLEL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB CORE_HEADERS
    ${CORE_INCLUDE_DIR}/Core/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${CORE_SOURCES} ${CORE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE Reflect Platform)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <string>
    <vector>
    <memory>
    <algorithm>
    <functional>
    <unordered_map>
    <limits>
    [["Core/ETLogger.hpp"]]
)