# C++ Standart Options

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Flags

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

    if(COMPILE_FLAGS_SATINE_MEMORY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif()

    if(COMPILE_FLAGS_PROFILE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    endif()

    set(CXX_APP_CHECK_FLAGS
-Wextra
-Wall
-Wundef
-Wshadow
-Wpointer-arith
-Wcast-align
-Wwrite-strings
-Wcast-qual
-Wswitch-default
-Wunreachable-code
-Werror CACHE INTERNAL "Compile flags for app")

# -Wswitch-enum
# -Wstrict-overflow=5
# -Wconversion

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