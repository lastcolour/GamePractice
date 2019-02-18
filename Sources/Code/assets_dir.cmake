if(NOT APP_ASSETS_ROOT_DIR)
    message(FATAL_ERROR "APP_ASSETS_ROOT_DIR path not specified")
else()
    message(STATUS "Assets dir: ${APP_ASSETS_ROOT_DIR}")
endif()

get_filename_component(ASSETS_ABSOLUTE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${APP_ASSETS_ROOT_DIR} ABSOLUTE)

if(NOT EXISTS ${ASSETS_ABSOLUTE_PATH})
    message(FATAL_ERROR "Can't find assets dir: ${ASSETS_ABSOLUTE_PATH}")
else()
    add_definitions(-DAPP_ASSETS_ROOT_DIR="${APP_ASSETS_ROOT_DIR}")
endif()