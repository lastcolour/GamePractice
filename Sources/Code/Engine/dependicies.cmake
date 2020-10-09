if(NOT MAIN_PROJECT_SOURCE_ROOT)
    message(FATAL_ERROR "Required cmake variable MAIN_PROJECT_SOURCE_ROOT not defined")
endif()

set(LIBS_ROOT_DIR ${MAIN_PROJECT_SOURCE_ROOT}/../Libs)
set(LIBS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../../Libs)

set(STB_INCLUDE_DIR ${LIBS_ROOT_DIR}/STB CACHE PATH "STB Include dir")
set(RAPID_JSON_INCLUDE_DIR ${LIBS_ROOT_DIR}/RapidJSON/include CACHE PATH "RapidJSON Include dir")

function(CreateStaticLibDependecy LibName LibIncludeDir LibLinkBinary)
    set(${LibName}_INCLUDE_DIR
        ${LibIncludeDir}
        CACHE PATH "${LibName} Include Dir")

    if(NOT EXISTS ${${LibName}_INCLUDE_DIR})
        message(FATAL_ERROR "Cant find include dir of libary: '${LibName}' at location: ${${LibName}_INCLUDE_DIR}")
    endif()

    set(${LibName}_LIB_STATIC_BIN
        ${LibLinkBinary}
        CACHE PATH "${LibName} Library Path")

    if(NOT EXISTS ${${LibName}_LIB_STATIC_BIN})
        message(FATAL_ERROR "Cant find link binary of libary: '${LibName}' at location: ${${LibName}_LIB_STATIC_BIN}")
    endif()

    message(STATUS "Create dependecy to static lib ${LibName} : ${${LibName}_LIB_STATIC_BIN}")
endfunction()

function(CreateSharedLibDependecy LibName LibIncludeDir LibLinkBinary)
    set(${LibName}_INCLUDE_DIR
        ${LibIncludeDir}
        CACHE PATH "${LibName} Include Dir")

    if(NOT EXISTS ${${LibName}_INCLUDE_DIR})
        message(FATAL_ERROR "Cant find include dir of libary: '${LibName}' at location: ${${LibName}_INCLUDE_DIR}")
    endif()

    set(${LibName}_LIB_SHARED_BIN
        ${CMAKE_CURRENT_BINARY_DIR}/../../${CMAKE_BUILD_TYPE}/${LibLinkBinary}
        CACHE PATH "${LibName} Library Path")

    if(NOT EXISTS ${${LibName}_LIB_SHARED_BIN})
        message(FATAL_ERROR "Cant find link binary of libary: '${LibName}' at location: ${${LibName}_LIB_SHARED_BIN}")
    endif()

    message(STATUS "Create dependecy to shared lib ${LibName} : ${${LibName}_LIB_SHARED_BIN}")
endfunction()

if(BUILD_PLATFORM STREQUAL "Linux")

    find_package(ZLIB REQUIRED)
    set(ZLIB_LIB_SHARED_BIN ${ZLIB_LIBRARIES} CACHE PATH "ZLib Library Path")

    find_package(Threads REQUIRED)
    set(THREADS_LIB_SHARED_BIN ${CMAKE_THREAD_LIBS_INIT} CACHE PATH "Thread Library Path")

    CreateSharedLibDependecy("GLAD"
        ${LIBS_OUTPUT_DIR}/GLAD/_cmake/include
        libglad.so
    )

    CreateSharedLibDependecy("FREETYPE2"
        ${LIBS_ROOT_DIR}/FreeType2/include
        libfreetype.so
    )

    CreateSharedLibDependecy("GLFW"
        ${LIBS_ROOT_DIR}/GLFW/include
        libglfw.so
    )

    CreateSharedLibDependecy("OPENAL_SOFT"
        ${LIBS_ROOT_DIR}/OpenAL-Soft/include
        libopenal.so
    )

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        CreateStaticLibDependecy("GTEST"
            ${LIBS_ROOT_DIR}/GTEST/googletest/include
            ${LIBS_OUTPUT_DIR}/GTEST/${CMAKE_BUILD_TYPE}/libgtestd.a
        )
    else()
        CreateStaticLibDependecy("GTEST"
            ${LIBS_ROOT_DIR}/GTEST/googletest/include
            ${LIBS_OUTPUT_DIR}/GTEST/${CMAKE_BUILD_TYPE}/libgtest.a
        )
    endif()

elseif(BUILD_PLATFORM STREQUAL "Windows")

    CreateStaticLibDependecy("GLAD"
        ${LIBS_OUTPUT_DIR}/GLAD/_cmake/include
        ${LIBS_OUTPUT_DIR}/GLAD/${CMAKE_BUILD_TYPE}/glad.lib
    )

    CreateStaticLibDependecy("FREETYPE2"
        ${LIBS_ROOT_DIR}/FreeType2/include
        ${LIBS_OUTPUT_DIR}/FreeType2/Release/freetype.lib
    )

    CreateStaticLibDependecy("GLFW"
        ${LIBS_ROOT_DIR}/GLFW/include
        ${LIBS_OUTPUT_DIR}/GLFW/Release/glfw3dll.lib
    )

    CreateStaticLibDependecy("OPENAL_SOFT"
        ${LIBS_ROOT_DIR}/OpenAL-Soft/include
        ${LIBS_OUTPUT_DIR}/OpenAL-Soft/Release/OpenAL32.lib
    )

    CreateStaticLibDependecy("ZLIB"
        ${LIBS_ROOT_DIR}/ZLIB
        ${LIBS_OUTPUT_DIR}/ZLIB/Release/zlibstatic.lib
    )

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        CreateStaticLibDependecy("GTEST"
            ${LIBS_ROOT_DIR}/GTEST/googletest/include
            ${LIBS_OUTPUT_DIR}/GTEST/${CMAKE_BUILD_TYPE}/gtestd.lib
        )
    else()
        CreateStaticLibDependecy("GTEST"
            ${LIBS_ROOT_DIR}/GTEST/googletest/include
            ${LIBS_OUTPUT_DIR}/GTEST/${CMAKE_BUILD_TYPE}/gtest.lib
        )
    endif()

elseif(BUILD_PLATFORM STREQUAL "Android")

    CreateStaticLibDependecy("OBOE"
        ${LIBS_ROOT_DIR}/Oboe/include
        ${LIBS_OUTPUT_DIR}/Oboe/Release/liboboe.a
    )

    CreateStaticLibDependecy("FREETYPE2"
        ${LIBS_ROOT_DIR}/FreeType2/include
        ${LIBS_OUTPUT_DIR}/FreeType2/Release/libfreetype.a
    )

endif()