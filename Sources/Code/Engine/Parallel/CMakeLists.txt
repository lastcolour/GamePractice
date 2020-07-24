cmake_minimum_required(VERSION 3.15)

project(Parallel CXX)

set(PARALLEL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Parallel include dir")

include_directories(
    ${PARALLEL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB PARALLEL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB PARALLEL_HEADERS
    ${PARALLEL_INCLUDE_DIR}/Parallel/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${PARALLEL_SOURCES} ${PARALLEL_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})