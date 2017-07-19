# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(rpcpp_samples)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# check package
include(FindPackages)
if(WIN32)
    include(windows_utils)
endif()

# subdirectory
set(rpcpp_samples_list "panda3d_samples" "render_pipeline_samples" "nvidia_flex"
    "01-Crytek-Sponza" "02-San-Miguel" "03-Shading-Models-VR"
)

foreach(sample_id ${rpcpp_samples_list})
    option(rpcpp_samples_BUILD_${sample_id} "Enable to build '${sample_id}'" OFF)
    if(rpcpp_samples_BUILD_${sample_id})
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${sample_id}")
    endif()
endforeach()
# ==================================================================================================
