cmake_minimum_required(VERSION 3.15)

project(Render CXX)

set(RENDER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Render include dir")

include_directories(
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${ENGINE_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${FREETYPE2_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB RENDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logics/*.cpp)

file(GLOB RENDER_HEADERS
    ${RENDER_INCLUDE_DIR}/Render/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${PROJECT_NAME} STATIC ${RENDER_SOURCES} ${RENDER_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

if(BUILD_PLATFORM STREQUAL "Windows")

    set(RENDER_LIB_DEPENDICIES
        ${FREETYPE2_LIB_STATIC_BIN}
        ${ZLIB_LIB_STATIC_BIN}
        CACHE INTERNAL "Render link dependecies")

elseif(BUILD_PLATFORM STREQUAL "Linux")

    set(RENDER_LIB_DEPENDICIES
        ${FREETYPE2_LIB_SHARED_BIN}
        ${ZLIB_LIB_SHARED_BIN}
        png
        CACHE INTERNAL "Render link dependecies")
else()

    set(RENDER_LIB_DEPENDICIES
        ${FREETYPE2_LIB_STATIC_BIN}
        GLESv3 EGL
        CACHE INTERNAL "Render link dependecies")

endif()

target_link_libraries(${PROJECT_NAME}
    ${RENDER_LIB_DEPENDICIES}
    Core)