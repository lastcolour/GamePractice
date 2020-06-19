cmake_minimum_required(VERSION 3.15)

project(Engine CXX)

include(dependicies.cmake)

if(BUILD_PLATFORM STREQUAL "Windows" OR BUILD_PLATFORM STREQUAL "Linux")
    set(ENGINE_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Desktop)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${APPLICATION_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR})

elseif(BUILD_PLATFORM STREQUAL "Android")
    set(ENGINE_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platforms/Android)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${APPLICATION_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR})

endif()

add_subdirectory(Core)
add_subdirectory(Render)
add_subdirectory(Audio)

include_directories(${CMAKE_INCLUDE_DIRECTORIES}
    ${CORE_INCLUDE_DIR}
    ${RENDER_INCLUDE_DIR}
    ${AUDIO_INCLUDE_DIR})

file(GLOB ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UI/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UI/Logics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Reflect/*.cpp
    ${ENGINE_PLATFORM_SOURCES}/*.cpp)

file(GLOB ENGINE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UI/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UI/Logics/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Reflect/*.hpp
    ${ENGINE_PLATFORM_SOURCES}/*.hpp)

add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_PDB_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
    ${ENGINE_LIB_DEPENDICIES}
    Core
    Render
    Audio)