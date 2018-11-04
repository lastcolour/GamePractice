cmake_minimum_required(VERSION 3.10)

project(${MAIN_PROJECT_NAME}Tests)

include_directories(${GTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MAIN_PROJECT_SOURCE_ROOT}/Application)

find_package(Threads REQUIRED)

file(GLOB APPLICATION_TESTS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTests/*.cpp)
file(GLOB APPLICATION_TESTS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTests/*.hpp)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests_main.cpp
    ${APPLICATION_TESTS_HEADERS}
    ${APPLICATION_TESTS_SOURCES})

target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIB_STATIC_BIN}
    ${CMAKE_THREAD_LIBS_INIT}
    Application)