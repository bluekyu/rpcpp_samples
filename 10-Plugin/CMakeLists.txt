# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(10-Plugin)

# === configure ====================================================================================
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

set(RP_PLUGIN_INSTALL_PREFIX "etc/render_pipeline/rpplugins")

# configure plugin
set(LIB_NAME "plugin")
set(LIB_TYPE MODULE)
add_definitions(-DPLUGIN_ID_STRING="${PROJECT_NAME}")

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    endif()
else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# include header directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")      # /
include_directories("${PROJECT_BINARY_DIR}")            # /build
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")  # ./src
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/src/sample_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/sample_stage.h"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp")
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} ${LIB_TYPE} ${plugin_sources})
target_link_libraries(${PROJECT_NAME}
    PUBLIC ${Boost_LIBRARIES} ${Panda3d_LIBRARIES}
    PRIVATE ${RenderPipeline_LIBRARIES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})

# library
install(TARGETS ${PROJECT_NAME} DESTINATION "${RP_PLUGIN_INSTALL_PREFIX}/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/config/" DESTINATION "${RP_PLUGIN_INSTALL_PREFIX}/${PROJECT_NAME}")
# ==================================================================================================
