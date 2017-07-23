# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(panda3d_samples)

# === configure ====================================================================================
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/looking-and-gripping")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/music-box")
# ==================================================================================================

# === install ======================================================================================
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/"
    DESTINATION "share/render_pipeline/resources/panda3d"
)
# ==================================================================================================
