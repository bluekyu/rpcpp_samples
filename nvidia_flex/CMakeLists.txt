# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(nvidia_flex)

# === configure ====================================================================================
# find flex
set(FLEX_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
set(FLEX_INCLUDEDIR "" CACHE PATH "Hint for finding NVIDIA flex include directory")
set(FLEX_LIBRARYDIR "" CACHE PATH "Hint for finding NVIDIA flex library directory")
find_package(Flex REQUIRED)

render_pipeline_FIND_PLUGINS("flex")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Initial-Overlap")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Instancing-Flex")
# ==================================================================================================
