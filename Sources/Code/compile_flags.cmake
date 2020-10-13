# C++ Standart Options

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Flags

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /EHsc /GR- /WX")
        set(CMAKE_CXX_FLAGS_DEBUG "/MD /Zi /Ob0 /Od /RTC1")
    endif()

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fPIC -Wall -pedantic -Wextra -Wno-unused-parameter")

    if(COMPILE_FLAGS_SANITIZE_MEMORY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif()

    if(COMPILE_FLAGS_PROFILE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    endif()

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

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

# Defines for platform

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(BUILD_PLATFORM "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(BUILD_PLATFORM "Linux")
    add_definitions(-DPLATFORM_LINUX)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(BUILD_PLATFORM "Android")
    add_definitions(-DPLATFORM_ANDROID)
endif()