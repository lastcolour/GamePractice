cmake_minimum_required(VERSION 3.16)

project(Audio CXX)

if(BUILD_PLATFORM STREQUAL "Windows" OR BUILD_PLATFORM STREQUAL "Linux")

    set(AUDIO_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Desktop)
    set(AUDIO_PLATFORM_INCLUDE_DIR ${OPENAL_SOFT_INCLUDE_DIR})

    if(BUILD_PLATFORM STREQUAL "Windows")
        set(AUDIO_LIB_DEPENDICIES
            ${OPENAL_SOFT_LIB_STATIC_BIN})
    else()
        set(AUDIO_LIB_DEPENDICIES
            ${OPENAL_SOFT_LIB_SHARED_BIN})
    endif()

elseif(BUILD_PLATFORM STREQUAL "Android")

    set(AUDIO_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Android)
    set(AUDIO_PLATFORM_INCLUDE_DIR ${OBOE_INCLUDE_DIR})
    set(AUDIO_LIB_DEPENDICIES
        ${OBOE_LIB_STATIC_BIN}
        OpenSLES)

endif()

include_directories(
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${AUDIO_PLATFORM_INCLUDE_DIR}
    ${ENTITY_INCLUDE_DIR}
    ${MATH_INCLUDE_DIR}
    ${REFLECT_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB AUDIO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${AUDIO_PLATFORM_SOURCES}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Nodes/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filters/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MixGraph/*.cpp)

file(GLOB AUDIO_HEADERS
    ${AUDIO_INCLUDE_DIR}/Audio/*.hpp
    ${AUDIO_PLATFORM_SOURCES}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Nodes/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filters/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MixGraph/*.hpp)

add_library(${PROJECT_NAME} STATIC ${AUDIO_SOURCES} ${AUDIO_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MixGraph/OggDataStream.cpp
            PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4245 /wd4456 /wd4457 /wd4701")

endif()

target_link_libraries(${PROJECT_NAME}
    ${AUDIO_LIB_DEPENDICIES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <string>
    <cstring>
    <vector>
    <memory>
    <algorithm>
    <functional>
    <unordered_map>
    <limits>
    [["Reflect/ReflectContext.hpp"]]
    [["Core/ETLogger.hpp"]]
    [["Entity/ETEntity.hpp"]]
)