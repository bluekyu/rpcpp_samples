# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.9)
project(rpcpp_samples)

# === configure ====================================================================================
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/OpenVR")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/OpenVR-AR")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/OpenVR-Tracked-Camera")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Panda3D-Chorusline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Panda3D-Shadows")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Shading-Models-VR")
# ==================================================================================================

# === install ======================================================================================
# ==================================================================================================
