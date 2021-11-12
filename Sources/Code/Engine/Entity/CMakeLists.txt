cmake_minimum_required(VERSION 3.16)

project(Entity CXX)

include_directories(
    ${ENTITY_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB ENTITY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB ENTITY_HEADERS
    ${ENTITY_INCLUDE_DIR}/Entity/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${ENTITY_SOURCES} ${ENTITY_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    [["Core/Core.hpp"]]
    [["Reflect/ReflectContext.hpp"]]
    [["Entity/ETEntity.hpp"]]
)