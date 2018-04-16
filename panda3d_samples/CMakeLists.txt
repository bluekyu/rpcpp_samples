# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.9)
project(panda3d_samples)

# === configure ====================================================================================
add_subdirectory("chorusline")
add_subdirectory("empty-wo-rpcpp")
add_subdirectory("looking-and-gripping")
add_subdirectory("music-box")
# ==================================================================================================

# === install ======================================================================================
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/"
    DESTINATION "share/render_pipeline/resources/panda3d"
)
# ==================================================================================================
