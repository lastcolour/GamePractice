cmake_minimum_required(VERSION 3.15)

project(Platform CXX)

set(PLATFORM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Platform include dir")

if(BUILD_PLATFORM STREQUAL "Windows")

    set(PLATFORM_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Desktop)

    include_directories(
        ${GLFW_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR})

    set(PLATFORM_LIB_DEPENDICIES
        ${GLFW_LIB_STATIC_BIN}
        ${GLAD_LIB_STATIC_BIN})

elseif(BUILD_PLATFORM STREQUAL "Linux")

    set(PLATFORM_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Desktop)

    include_directories(
        ${GLFW_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR})

    set(PLATFORM_LIB_DEPENDICIES
        ${GLFW_LIB_SHARED_BIN}
        ${GLAD_LIB_SHARED_BIN}
        dl)
else()

    set(PLATFORM_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Android)

    include_directories(
        ${ENGINE_INCLUDE_DIR})

    set(PLATFORM_LIB_DEPENDICIES
        Engine
        log android z)

endif()

include_directories(
    ${CMAKE_INCLUDE_DIRECTORIES}
    ${MATH_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB PLATFORM_SOURCES
    ${PLATFORM_SOURCES_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB PLATFORM_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${PLATFORM_INCLUDE_DIR}/Platform/*.hpp
    ${PLATFORM_SOURCES_DIR}/*.hpp)

add_library(${PROJECT_NAME} STATIC ${PLATFORM_SOURCES} ${PLATFORM_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    ${PLATFORM_LIB_DEPENDICIES})