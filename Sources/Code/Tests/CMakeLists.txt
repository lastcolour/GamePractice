cmake_minimum_required(VERSION 3.15)

project(${MAIN_PROJECT_NAME}Tests CXX)

find_package(Threads REQUIRED)

add_subdirectory(TestUtils)
add_subdirectory(CoreTests)

include_directories(${GTEST_INCLUDE_DIR}
    ${TEST_UTILS_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests_main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIB_STATIC_BIN}
    ${CMAKE_THREAD_LIBS_INIT}
    TestUtils
    CoreTests
    Engine
    Application)