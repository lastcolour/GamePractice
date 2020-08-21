cmake_minimum_required(VERSION 3.15)

project(Reflect CXX)

set(REFLECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Reflect include dir")

include_directories(
    ${REFLECT_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB REFLECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB REFLECT_HEADERS
    ${MATH_INCLUDE_DIR}/Reflect/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${REFLECT_SOURCES} ${REFLECT_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    Core)