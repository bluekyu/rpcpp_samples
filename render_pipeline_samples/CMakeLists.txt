# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)
project(render_pipeline_samples
    LANGUAGES NONE
)

# === configure ====================================================================================
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/00-Loading-The-Pipeline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/01-Material-Demo")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/02-Roaming-Ralph")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/03-Lights")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/06-Car")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/07-Shading-Models")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/08-Terrain")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/10-Plugin")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/11-Instancing")
# ==================================================================================================

# === install ======================================================================================
# ==================================================================================================
