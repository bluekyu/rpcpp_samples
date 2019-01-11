# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.12)
project(rpcpp_samples
    LANGUAGES NONE
)

# === configure ====================================================================================
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Assimp-Models")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ImGui")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/OpenVR")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/OpenVR-AR")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/OpenVR-Tracked-Camera")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Panda3D-Chorusline")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Panda3D-Shadows")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Shading-Models-VR")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ViveSR")
# ==================================================================================================

# === install ======================================================================================
# ==================================================================================================
