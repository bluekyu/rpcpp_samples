# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

get_filename_component(RPPLUGINS_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugins_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Sample plugin for Render Pipeline C++"
    LANGUAGES CXX
)

# === configure ====================================================================================
option(${PROJECT_NAME}_ENABLE_RTTI "Enable Run-Time Type Information" OFF)

include(debugging-information)

# check installing directory
render_pipeline_check_install_directory()

set(RPPLUGINS_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGINS_ID}")
set(RPPLUGINS_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugins-config.cmake.in")

# === project specific packages ===

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/src/sample_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/sample_stage.h"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.hpp"
)
add_library(${PROJECT_NAME} MODULE ${plugin_sources})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251 /utf-8
        $<$<NOT:$<BOOL:${${PROJECT_NAME}_ENABLE_RTTI}>>:/GR->
        $<$<VERSION_GREATER:${MSVC_VERSION},1900>:/permissive->

        # note: windows.cmake in vcpkg
        $<$<CONFIG:Release>:/Oi /Gy /Z7>
    )

    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO    " /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
    set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE           " /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF ")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall
        $<$<NOT:$<BOOL:${${PROJECT_NAME}_ENABLE_RTTI}>>:-fno-rtti>
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPPLUGINS_ID_STRING="${RPPLUGINS_ID}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC render_pipeline::render_pipeline
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "rpcpp_samples"
    DEBUG_POSTFIX ${render_pipeline_DEBUG_POSTFIX}
    RELWITHDEBINFO_POSTFIX ${render_pipeline_RELWITHDEBINFO_POSTFIX}
    VERSION ${PROJECT_VERSION}
)

configure_debugging_information(TARGET ${PROJECT_NAME} CONFIGURATIONS Debug RelWithDebInfo Release)
# ==================================================================================================

# === target =======================================================================================
# create plugin interface target
add_library(${RPPLUGINS_ID} INTERFACE)

target_include_directories(${RPPLUGINS_ID}
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${RPPLUGINS_INSTALL_DIR}/include>
)

target_link_libraries(${RPPLUGINS_ID}
    INTERFACE render_pipeline::render_pipeline
)

# configure package
set(PACKAGE_NAME "rpplugins_${RPPLUGINS_ID}")
set(TARGET_NAMESPACE "rpplugins::")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "${RPPLUGINS_INSTALL_DIR}/cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
)
configure_package_config_file(${RPPLUGINS_CONFIG_IN_FILE}
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

add_library(${TARGET_NAMESPACE}${RPPLUGINS_ID} ALIAS ${RPPLUGINS_ID})
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME} DESTINATION ${RPPLUGINS_INSTALL_DIR})
install(TARGETS ${RPPLUGINS_ID} EXPORT ${TARGET_EXPORT_NAME})
export(EXPORT ${TARGET_EXPORT_NAME}
    NAMESPACE ${TARGET_NAMESPACE}
    FILE "${PROJECT_BINARY_DIR}/${TARGET_EXPORT_NAME}.cmake"
)

install(FILES "${PROJECT_SOURCE_DIR}/config.yaml" DESTINATION ${RPPLUGINS_INSTALL_DIR})
foreach(directory_name "include" "resources" "shader")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/${directory_name}"
        DESTINATION ${RPPLUGINS_INSTALL_DIR}
        OPTIONAL
    )
endforeach()

install(FILES ${PACKAGE_CONFIG_FILE} ${PACKAGE_VERSION_CONFIG_FILE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})
install(FILES ${${PROJECT_NAME}_MACRO_CMAKE_FILE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR} OPTIONAL)
install(EXPORT ${TARGET_EXPORT_NAME} NAMESPACE ${TARGET_NAMESPACE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})

install_debugging_information(TARGET ${PROJECT_NAME}
    DESTINATION "${RPPLUGINS_INSTALL_DIR}"
    CONFIGURATIONS Debug RelWithDebInfo Release
)
# ==================================================================================================
