# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(rpcpp_samples)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# check package
include(FindPackages)

# subdirectory
add_subdirectory("01-Crytek-Sponza")
add_subdirectory("02-San-Miguel")
add_subdirectory("03-Shading-Models-VR")
add_subdirectory("nvidia_flex")
add_subdirectory("panda3d_samples")
add_subdirectory("render_pipeline_samples")
# ==================================================================================================
