cmake_minimum_required(VERSION 3.16)

project(Math CXX)

include_directories(
    ${MATH_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB MATH_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB MATH_HEADERS
    ${MATH_INCLUDE_DIR}/Math/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${MATH_SOURCES} ${MATH_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <string>
    <vector>
    <memory>
    <algorithm>
    <functional>
    <unordered_map>
    <limits>
)