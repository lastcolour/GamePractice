if(NOT MAIN_PROJECT_SOURCE_ROOT)
    message(FATAL_ERROR "Required cmake variable MAIN_PROJECT_SOURCE_ROOT not defined")
endif()

set(LIBS_ROOT_DIR ${MAIN_PROJECT_SOURCE_ROOT}/../Libs)

if(APP_BUILD_PLATFORM STREQUAL "Windows")

    set(LIB_PREFIX "")
    set(LIB_SHARED_EXTENSION ".lib")
    set(LIB_STATIC_EXTENSION ".lib")

    set(GLAD_LIBRARY_NAME glad)
    set(GLFW_LIBRARY_NAME glfw3dll)
    set(FREETYPE2_LIBRARY_NAME freetype)
    set(OPENAL_SOFT_LIBRARY_NAME OpenAL32)
    set(GTEST_LIBRRY_NAME gtestd)
    set(ZLIB_LIBRARY_NAME zlib)

elseif(APP_BUILD_PLATFORM STREQUAL "Linux" OR APP_BUILD_PLATFORM STREQUAL "Android")

    set(LIB_PREFIX "lib")
    set(LIB_STATIC_EXTENSION ".a")
    set(LIB_SHARED_EXTENSION ".so")

    set(GLAD_LIBRARY_NAME glad)
    set(GLFW_LIBRARY_NAME glfw)
    set(FREETYPE2_LIBRARY_NAME freetype)
    set(OPENAL_SOFT_LIBRARY_NAME openal)
    set(GTEST_LIBRRY_NAME gtestd)

endif()

set(RAPID_JSON_INCLUDE_DIR ${LIBS_ROOT_DIR}/RapidJSON/include CACHE PATH "RapidJSON Include dir")

set(GLFW_INCLUDE_DIR ${LIBS_ROOT_DIR}/GLFW/include CACHE PATH GLFW "Include dir")
set(GLFW_LIB_DYNANIC_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_PREFIX}${GLFW_LIBRARY_NAME}${LIB_SHARED_EXTENSION}
    CACHE PATH "GLFW Library Path")

set(GLAD_INCLUDE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GLAD/include CACHE PATH "GLAD Include dir")
set(GLAD_LIB_STATIC_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_PREFIX}${GLAD_LIBRARY_NAME}${LIB_STATIC_EXTENSION}
    CACHE PATH "GLAD Library Path")

set(FREETYPE2_INCLUDE_DIR ${LIBS_ROOT_DIR}/FreeType2/include CACHE PATH "FreeType2 Include dir")
set(FREETYPE2_LIB_STATIC_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_PREFIX}${FREETYPE2_LIBRARY_NAME}${LIB_STATIC_EXTENSION}
    CACHE PATH "FreeType2 Library Path")

set(OPENAL_SOFT_INCLUDE_DIR ${LIBS_ROOT_DIR}/OpenAL-Soft/include CACHE PATH "OpenAL-Soft Include dir")
set(OPENAL_SOFT_LIB_SHARED_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_PREFIX}${OPENAL_SOFT_LIBRARY_NAME}${LIB_SHARED_EXTENSION}
    CACHE PATH "OpenAL-Soft Library Path")

set(STB_INCLUDE_DIR ${LIBS_ROOT_DIR}/STB CACHE PATH "STB Include dir")

if(APP_BUILD_PLATFORM STREQUAL "Linux")
    find_package(ZLIB REQUIRED)
    set(ZLIB_LIB_SHARED_BIN ${ZLIB_LIBRARIES} CACHE PATH "ZLib Library Path")
elseif(APP_BUILD_PLATFORM STREQUAL "Windows")
    set(ZLIB_LIB_SHARED_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_PREFIX}${ZLIB_LIBRARY_NAME}${LIB_SHARED_EXTENSION}
    CACHE PATH "ZLib Library Path")
endif()

if(NOT APP_BUILD_PLATFORM STREQUAL "Android")

    set(APP_LIB_DEPENDICIES
        ${GLFW_LIB_DYNANIC_BIN}
        ${GLAD_LIB_STATIC_BIN}
        ${FREETYPE2_LIB_STATIC_BIN}
        ${ZLIB_LIB_SHARED_BIN}
        ${OPENAL_SOFT_LIB_SHARED_BIN}
        CACHE INTERNAL "Application link depedicies")

    if(APP_BUILD_PLATFORM STREQUAL "Linux")
        set(APP_LIB_DEPENDICIES ${APP_LIB_DEPENDICIES} dl png)
    endif()

else()

    set(APP_LIB_DEPENDICIES
        ${FREETYPE2_LIB_STATIC_BIN}
        GLESv3 EGL log android z
        CACHE INTERNAL "Application link depedicies")

endif()

if(BUILD_UNITTESTS)
    set(GTEST_PROJECT_PATH ${LIBS_ROOT_DIR}/GTEST)
    set(GTEST_INCLUDE_DIR ${GTEST_PROJECT_PATH}/googletest/include CACHE PATH "GoogleTest include dir")
    set(GTEST_LIB_STATIC_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIB_PREFIX}${GTEST_LIBRRY_NAME}${LIB_STATIC_EXTENSION}
        CACHE PATH "GTEST Library Path")
endif()
