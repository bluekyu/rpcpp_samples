# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(render_pipeline_samples)

# === configure ====================================================================================
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/00-Loading-The-Pipeline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Material-Demo")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/03-Lights")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/06-Car")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/07-Shading-Models")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/10-Plugin")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/11-Instancing")
# ==================================================================================================

# === install ======================================================================================
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config/" DESTINATION "etc/rpsamples/default")
# ==================================================================================================
