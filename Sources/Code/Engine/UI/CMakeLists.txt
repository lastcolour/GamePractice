cmake_minimum_required(VERSION 3.15)

project(UI CXX)

include_directories(
    ${ENTITY_INCLUDE_DIR}
    ${UI_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR}
    ${APPLICATION_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Config/*.cpp)

file(GLOB UI_HEADERS
    ${UI_INCLUDE_DIR}/UI/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logics/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Config/*.hpp)

add_library(${PROJECT_NAME} STATIC ${UI_SOURCES} ${UI_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})