cmake_minimum_required(VERSION 3.15)

project(Core CXX)

set(CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Core include dir")

include_directories(
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${RAPID_JSON_INCLUDE_DIR}
    ${APPLICATION_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB CORE_HEADERS
    ${CORE_INCLUDE_DIR}/Core/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${CORE_SOURCES} ${CORE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    Engine
    Application)