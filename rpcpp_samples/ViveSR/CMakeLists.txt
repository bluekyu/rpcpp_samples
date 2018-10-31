# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)
project(ViveSR
    DESCRIPTION "Samples for ViveSR"
    LANGUAGES CXX
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === project specific packages ===
find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)                 # for libfmt in ubuntu package
    set(FMT_TARGET fmt::fmt-header-only)
else()
    set(FMT_TARGET fmt::fmt)
endif()

if(NOT TARGET OpenVR::OpenVR)
    find_package(OpenVR)
    if(NOT TARGET OpenVR::OpenVR)
        message(STATUS "\n  Failed to find 'OpenVR' package. ${PROJECT_NAME} project will be disabled.\n")
        return()
    endif()
endif()

find_package(imgui CONFIG)
if(NOT TARGET imgui::imgui)
    message(STATUS "\n  Failed to find 'imgui' package. ${PROJECT_NAME} project will be disabled.\n")
    return()
endif()
set_target_properties(imgui::imgui PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

render_pipeline_find_plugins("openvr;vivesr;ar_render")
if(NOT (TARGET rpplugins::openvr AND TARGET rpplugins::vivesr AND TARGET rpplugins::ar_render))
    message(STATUS "  ${PROJECT_NAME} project will be disabled.\n")
    return()
endif()
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(sources_files
    "${PROJECT_SOURCE_DIR}/src/main.hpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# grouping
#source_group("src" FILES ${source_})
# ==================================================================================================

# === target =======================================================================================
add_executable(${PROJECT_NAME} ${sources_files})

if(MSVC)
    windows_add_longpath_manifest(${PROJECT_NAME})
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /utf-8
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/permissive->

        # note: windows.cmake in vcpkg
        $<$<CONFIG:Release>:/Oi /Gy /Z7>
    )
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
        $<$<NOT:$<BOOL:${rpcpp_samples_project_ENABLE_RTTI}>>:-fno-rtti>
    )
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline::render_pipeline rpplugins::openvr rpplugins::vivesr rpplugins::ar_render
    ${FMT_TARGET} imgui::imgui
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "rpcpp_samples/rpcpp_samples")
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
# ==================================================================================================
