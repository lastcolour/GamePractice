cmake_minimum_required(VERSION 3.16)

project(Parallel CXX)

include_directories(
    ${PARALLEL_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB PARALLEL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB PARALLEL_HEADERS
    ${PARALLEL_INCLUDE_DIR}/Parallel/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${PARALLEL_SOURCES} ${PARALLEL_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE ${THREADS_LIB_SHARED_BIN})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    [["Core/Core.hpp"]]
)