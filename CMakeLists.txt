# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(rp-cpp-samples)

# === configure ====================================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# check package
include(FindPackages)

# subdirectory
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/00-Loading-The-Pipeline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Material-Demo")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/03-Lights")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/06-Car")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/07-Shading-Models")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/10-Plugin")
# ==================================================================================================
