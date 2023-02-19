cmake_minimum_required(VERSION 3.16)

project(${MAIN_PROJECT_NAME}Tests CXX)

set(TESTS_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${GTEST_INCLUDE_DIR}
    ${TESTS_SOURCES_DIR}
    ${CORE_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Core/src
    ${RENDER_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Render/src
    ${ENGINE_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Audio/src
    ${ENTITY_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Entity/src
    ${UI_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/UI/src
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Platform/src
    ${PARALLEL_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}/Parallel/src
    ${APPLICATION_INCLUDE_DIR})

file(GLOB TESTS_SOURCES ${TESTS_SOURCES_DIR}/*.cpp
    ${TESTS_SOURCES_DIR}/TestUtils/*.cpp
    ${TESTS_SOURCES_DIR}/RenderTests/*.cpp)

file(GLOB TESTS_HEADERS ${TESTS_SOURCES_DIR}/*.hpp
    ${TESTS_SOURCES_DIR}/TestUtils/*.hpp
    ${TESTS_SOURCES_DIR}/RenderTests/*.hpp)

add_executable(${PROJECT_NAME}
    ${TESTS_SOURCES}
    ${TESTS_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests_main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RESULT_OUTPUT_DIR}/Debug)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RESULT_OUTPUT_DIR}/Release)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${RESULT_OUTPUT_DIR}/RelWithDebInfo)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(status "Disable MSVC secure warnings for tests")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GTEST_LIB_STATIC_BIN}
    ${THREADS_LIB_SHARED_BIN}
    Engine)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    [["Core/Core.hpp"]]
    [["Entity/ETEntity.hpp"]]
)