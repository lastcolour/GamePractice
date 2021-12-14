cmake_minimum_required(VERSION 3.16)

project(Render CXX)

include_directories(
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${FREETYPE2_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${PARALLEL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB RENDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Particles/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Debug/*.cpp)

file(GLOB RENDER_HEADERS
    ${RENDER_INCLUDE_DIR}/Render/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logics/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Particles/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Debug/*.hpp)

add_library(${PROJECT_NAME} STATIC ${RENDER_SOURCES} ${RENDER_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

if(BUILD_PLATFORM STREQUAL "Windows")

    set(RENDER_LIB_DEPENDICIES
        ${FREETYPE2_LIB_STATIC_BIN}
        ${ZLIB_LIB_STATIC_BIN}
        ${GLAD_LIB_STATIC_BIN}
        CACHE INTERNAL "Render link dependecies")

elseif(BUILD_PLATFORM STREQUAL "Linux")

    set(RENDER_LIB_DEPENDICIES
        ${FREETYPE2_LIB_SHARED_BIN}
        ${ZLIB_LIB_SHARED_BIN}
        ${GLAD_LIB_SHARED_BIN}
        png
        CACHE INTERNAL "Render link dependecies")

else()

    set(RENDER_LIB_DEPENDICIES
        ${FREETYPE2_LIB_STATIC_BIN}
        GLESv3 EGL
        CACHE INTERNAL "Render link dependecies")

endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${RENDER_LIB_DEPENDICIES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    [["Core/Core.hpp"]]
    [["Reflect/ReflectContext.hpp"]]
    [["Entity/ETEntity.hpp"]]
)