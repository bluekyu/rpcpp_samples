#.rst:
# FindViveSR
# --------------
#
# FindViveSR.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2018-06-11
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   ViveSR_FOUND            - True if ViveSR has been found and can be used
#
# and the following imported targets::
#
#   ViveSR::ViveSR          - The ViveSR library

cmake_minimum_required(VERSION 3.6)

set(ViveSR_ROOT "${ViveSR_ROOT}" CACHE PATH "Hint for finding ViveSR root directory")

find_path(ViveSR_INCLUDE_DIR
    NAMES "ViveSR_API.h"
    HINTS "${ViveSR_ROOT}"
    PATH_SUFFIXES include headers
    NO_DEFAULT_PATH
)

find_library(ViveSR_LIBRARY
    NAMES ViveSR_API
    HINTS "${ViveSR_ROOT}"
    PATH_SUFFIXES lib "libs"
    NO_DEFAULT_PATH
)

# Set ViveSR_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ViveSR
    FOUND_VAR ViveSR_FOUND
    REQUIRED_VARS ViveSR_LIBRARY ViveSR_INCLUDE_DIR
)

if(ViveSR_FOUND)
    message(STATUS "Found the ViveSR")

    add_library(ViveSR::ViveSR UNKNOWN IMPORTED)

    if(EXISTS "${ViveSR_LIBRARY}")
        set_target_properties(ViveSR::ViveSR PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${ViveSR_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${ViveSR_LIBRARY}"
        )
    endif()

    # Make variables changeable to the advanced user
    mark_as_advanced(
        ViveSR_INCLUDE_DIR
        ViveSR_LIBRARY
        ViveSR_ROOT
    )
endif()
