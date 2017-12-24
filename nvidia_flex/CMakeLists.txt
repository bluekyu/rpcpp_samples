# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.9)
project(nvidia_flex)

# === configure ====================================================================================
if(NOT TARGET NvFlex::CUDA)
    set(NvFlex_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
    find_package(NvFlex REQUIRED)
endif()

render_pipeline_find_plugins("rpflex")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Flex-Adhesion")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Flex-Dam-Break")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Flex-Friction-Ramp")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Flex-Initial-Overlap")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Flex-Instancing-Flex")
# ==================================================================================================

# === install ======================================================================================
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/config/" DESTINATION "etc/rpsamples/rpflex")
# ==================================================================================================
