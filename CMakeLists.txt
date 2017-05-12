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
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/00-Loading-The-Pipeline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Material-Demo")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/03-Lights")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/06-Car")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/07-Shading-Models")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/10-Plugin")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/11-Instancing")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/11-Crytek-Sponza")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/12-San-Miguel")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/13-Shading-Models-VR")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/20-Instancing-Flex")
# ==================================================================================================
