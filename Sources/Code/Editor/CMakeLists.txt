cmake_minimum_required(VERSION 3.15)

project(Editor CXX)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${ENGINE_INCLUDE_DIR}
    ${APPLICATION_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR})

file(GLOB EDITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB EDITOR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} SHARED ${EDITOR_SOURCES} ${EDITOR_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    Application
    Engine)