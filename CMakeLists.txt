# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)
project(rpcpp_samples_project
    DESCRIPTION "Samples of Render Pipeline C++"
    LANGUAGES CXX
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# check package
if(NOT TARGET render_pipeline)
    find_package(render_pipeline CONFIG REQUIRED)
endif()

if(WIN32)
    include(windows_utils)
endif()

# subdirectory
set(rpcpp_samples_list
    "nvidia_flex"
    "panda3d_samples"
    "render_pipeline_samples"
    "rpcpp_samples"
    "snippets"
)

foreach(sample_id ${rpcpp_samples_list})
    option(rpcpp_samples_BUILD_${sample_id} "Enable to build '${sample_id}'" OFF)
    if(rpcpp_samples_BUILD_${sample_id})
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${sample_id}")
    endif()
endforeach()
# ==================================================================================================

# === install ======================================================================================
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/models"
    DESTINATION "share/rpcpp_samples"
)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/etc/" DESTINATION "etc")
# ==================================================================================================
