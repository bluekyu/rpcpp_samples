# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(nvidia_flex)

# === configure ====================================================================================
if(NOT TARGET NvFlex::CUDA)
    set(NvFlex_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
    find_package(NvFlex REQUIRED)
endif()

render_pipeline_FIND_PLUGINS("flex")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Initial-Overlap")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Instancing-Flex")
# ==================================================================================================
