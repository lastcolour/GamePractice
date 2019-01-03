# C++ Standart Options

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Flags

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

else()
    message(FATAL_ERROR "Unknown compiler id: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Project CXX build flags: [${CMAKE_CXX_FLAGS}, ${CMAKE_CXX_FLAGS_DEBUG}]")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Project CXX build flags: [${CMAKE_CXX_FLAGS}, ${CMAKE_CXX_FLAGS_RELEASE}]")
else()
    message(STATUS "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()