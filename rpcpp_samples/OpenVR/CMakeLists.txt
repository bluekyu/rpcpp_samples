# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)
project(OpenVR
    DESCRIPTION "OpenVR sample"
    LANGUAGES CXX
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === project specific packages ===
if(NOT TARGET OpenVR::OpenVR)
    # find OpenVR
    set(OpenVR_ROOT "" CACHE PATH "Hint for finding OpenVR root directory")
    find_package(OpenVR REQUIRED)
endif()

if (NOT TARGET spdlog::spdlog)
    find_package(spdlog)
endif()

render_pipeline_find_plugins("openvr")
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(sources_files
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/world.cpp"
    "${PROJECT_SOURCE_DIR}/src/world.hpp"
)

# grouping
#source_group("src" FILES ${source_})
# ==================================================================================================

# === target =======================================================================================
add_executable(${PROJECT_NAME} ${sources_files})

if(MSVC)
    windows_add_longpath_manifest(${PROJECT_NAME})
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251
        $<$<VERSION_GREATER:${MSVC_VERSION},1800>:/utf-8>
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/permissive->
    )
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline::render_pipeline rpplugins::openvr spdlog::spdlog
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "rpcpp_samples/rpcpp_samples")
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
# ==================================================================================================
