# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(Flex-Dam-Break)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === project specific packages ===
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(sources_files
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/scene.hpp"
    "${PROJECT_SOURCE_DIR}/../common/helpers.hpp"
    "${PROJECT_SOURCE_DIR}/../common/maths.hpp"
    "${PROJECT_SOURCE_DIR}/../common/mesh.hpp"
)

# grouping
#source_group("src" FILES ${source_})
# ==================================================================================================

# === target =======================================================================================
add_executable(${PROJECT_NAME} ${sources_files})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type)
if(MSVC)
    windows_add_longpath_manifest(${PROJECT_NAME})
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/../common"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline::render_pipeline Boost::filesystem
    rpplugin::rpflex
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "rpcpp_samples/nvidia_flex")
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
# ==================================================================================================
