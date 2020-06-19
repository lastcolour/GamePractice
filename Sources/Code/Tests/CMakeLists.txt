cmake_minimum_required(VERSION 3.15)

project(${MAIN_PROJECT_NAME}Tests CXX)

find_package(Threads REQUIRED)

set(TESTS_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTests)

include_directories(${GTEST_INCLUDE_DIR}
    ${TESTS_SOURCES_DIR}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Render/src
    ${ENGINE_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${APPLICATION_INCLUDE_DIR})

file(GLOB TESTS_SOURCES ${TESTS_SOURCES_DIR}/*.cpp
    ${TESTS_SOURCES_DIR}/TestUtils/*.cpp)

file(GLOB TESTS_HEADERS ${TESTS_SOURCES_DIR}/*.hpp
    ${TESTS_SOURCES_DIR}/TestUtils/*.hpp)

add_executable(${PROJECT_NAME}
    ${TESTS_SOURCES}
    ${TESTS_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests_main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIB_STATIC_BIN}
    ${CMAKE_THREAD_LIBS_INIT}
    Engine
    Application)