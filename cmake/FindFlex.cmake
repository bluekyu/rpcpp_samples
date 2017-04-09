#.rst:
# FindFlex
# --------------
#
# FindFlex.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   Flex_INCLUDE_DIRS     - Include directories for NVIDIA Flex
#   Flex_LIBRARIES        - Libraries to link against NVIDIA Flex
#   Flex_FOUND            - True if NVIDIA Flex has been found and can be used
#   Flex_VERSION          - Library version for NVIDIA Flex
#   Flex_VERSION_STRING   - Library version for NVIDIA Flex
#   Flex_VERSION_MAJOR    - Library major version for NVIDIA Flex
#   Flex_VERSION_MINOR    - Library minor version for NVIDIA Flex
#   Flex_VERSION_PATCH    - Library patch version for NVIDIA Flex
#
# and the following imported targets::
#
#   Flex::Flex  - The NVIDIA Flex library

cmake_minimum_required(VERSION 3.2)

find_path(Flex_INCLUDE_DIR
    NAMES "NvFlex.h"
    HINTS ${FLEX_INCLUDEDIR} "${FLEX_ROOT}/include"
)
find_library(Flex_LIBRARY_RELEASE_CUDA
    NAMES NvFlexReleaseCUDA_x64
    HINTS ${FLEX_LIBRARYDIR} "${FLEX_ROOT}/lib"
    PATH_SUFFIXES win64
)
find_library(Flex_LIBRARY_EXT_RELEASE_CUDA
    NAMES NvFlexExtReleaseCUDA_x64
    HINTS ${FLEX_LIBRARYDIR} "${FLEX_ROOT}/lib"
    PATH_SUFFIXES win64
)
find_library(Flex_LIBRARY_DEVICE_RELEASE
    NAMES NvFlexDeviceRelease_x64
    HINTS ${FLEX_LIBRARYDIR} "${FLEX_ROOT}/lib"
    PATH_SUFFIXES win64
)
find_library(Flex_LIBRARY_DEBUG_CUDA
    NAMES NvFlexDebugCUDA_x64
    HINTS ${FLEX_LIBRARYDIR} "${FLEX_ROOT}/lib"
    PATH_SUFFIXES win64
)
find_library(Flex_LIBRARY_EXT_DEBUG_CUDA
    NAMES NvFlexExtDebugCUDA_x64
    HINTS ${FLEX_LIBRARYDIR} "${FLEX_ROOT}/lib"
    PATH_SUFFIXES win64
)
find_library(Flex_LIBRARY_DEVICE_DEBUG
    NAMES NvFlexDeviceDebug_x64
    HINTS ${FLEX_LIBRARYDIR} "${FLEX_ROOT}/lib"
    PATH_SUFFIXES win64
)

# Set Flex_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Flex
    FOUND_VAR Flex_FOUND
    REQUIRED_VARS Flex_LIBRARY_RELEASE_CUDA Flex_LIBRARY_EXT_RELEASE_CUDA Flex_LIBRARY_DEVICE_RELEASE Flex_INCLUDE_DIR
    VERSION_VAR Flex_VERSION
)

if(Flex_FOUND)
    # found both
    if(Flex_LIBRARY_RELEASE_CUDA AND Flex_LIBRARY_DEBUG_CUDA)
        set(Flex_LIBRARIES
            optimized               ${Flex_LIBRARY_RELEASE_CUDA} ${Flex_LIBRARY_EXT_RELEASE_CUDA} ${Flex_LIBRARY_DEVICE_RELEASE}
            debug                   ${Flex_LIBRARY_DEBUG_CUDA} ${Flex_LIBRARY_EXT_DEBUG_CUDA} ${Flex_LIBRARY_DEVICE_DEBUG})
    # found only release
    elseif(Flex_LIBRARY_RELEASE_CUDA)
        set(Flex_LIBRARIES    ${Flex_LIBRARY_RELEASE_CUDA} ${Flex_LIBRARY_EXT_RELEASE_CUDA} ${Flex_LIBRARY_DEVICE_RELEASE})
    # found only debug
    elseif(Flex_LIBRARY_DEBUG_CUDA)
        set(Flex_LIBRARIES    ${Flex_LIBRARY_DEBUG_CUDA} ${Flex_LIBRARY_EXT_DEBUG_CUDA} ${Flex_LIBRARY_DEVICE_DEBUG})
    endif()
    set(Flex_INCLUDE_DIRS ${Flex_INCLUDE_DIR})
endif()

mark_as_advanced(
    Flex_INCLUDE_DIRS
    Flex_LIBRARIES
)
