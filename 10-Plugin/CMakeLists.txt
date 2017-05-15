# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID})

# === configure ====================================================================================
file(RELATIVE_PATH RPPLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")
set(RPPLUGIN_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugin-config.cmake.in")

set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# === project specific packages ===
# ==================================================================================================

# === target =======================================================================================
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/src/sample_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/sample_stage.h"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.hpp"
)
add_library(${PROJECT_NAME} MODULE ${plugin_sources})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPPLUGIN_ID_STRING="${RPPLUGIN_ID}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "render_pipeline/rpplugins"
    OUTPUT_NAME "plugin"
)

# create plugin interface target
add_library(${RPPLUGIN_ID} INTERFACE)

target_include_directories(${RPPLUGIN_ID}
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${RPPLUGIN_INSTALL_DIR}/include>
)

target_link_libraries(${RPPLUGIN_ID}
    INTERFACE render_pipeline
)

# configure package
set(PACKAGE_NAME "rpplugin_${RPPLUGIN_ID}")
set(TARGET_NAMESPACE "rpplugin::")
set(PACKAGE_VERSION_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake")
set(PACKAGE_CONFIG_FILE "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake")
set(TARGET_EXPORT_NAME "${PACKAGE_NAME}-targets")
set(PACKAGE_CMAKE_INSTALL_DIR "${RPPLUGIN_INSTALL_DIR}/cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PACKAGE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(${RPPLUGIN_CONFIG_IN_FILE}
    ${PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR}
)

add_library(${TARGET_NAMESPACE}${RPPLUGIN_ID} ALIAS ${RPPLUGIN_ID})
export(TARGETS ${RPPLUGIN_ID}
    NAMESPACE ${TARGET_NAMESPACE}
    FILE "${PROJECT_BINARY_DIR}/${TARGET_EXPORT_NAME}.cmake"
)
export(PACKAGE ${PACKAGE_NAME})
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME} DESTINATION ${RPPLUGIN_INSTALL_DIR})
install(TARGETS ${RPPLUGIN_ID} EXPORT ${TARGET_EXPORT_NAME})

install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/" DESTINATION ${RPPLUGIN_INSTALL_DIR})
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION ${RPPLUGIN_INSTALL_DIR})
endif()

install(FILES ${PACKAGE_CONFIG_FILE} ${PACKAGE_VERSION_CONFIG_FILE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})
install(EXPORT ${TARGET_EXPORT_NAME} NAMESPACE ${TARGET_NAMESPACE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR})
# ==================================================================================================
