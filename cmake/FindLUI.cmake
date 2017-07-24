#.rst:
# FindLUI
# --------------
#
# FindLUI.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-19
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   LUI_FOUND            - True if LUI has been found and can be used
#
# and the following imported targets::
#
#   lui                  - The LUI library

cmake_minimum_required(VERSION 3.6)

find_path(LUI_INCLUDE_DIR
    NAMES "config_lui.h"
    HINTS "${LUI_ROOT}"
    PATH_SUFFIXES "include"
)

find_library(LUI_LIBRARY
    NAMES lui
    HINTS "${LUI_ROOT}"
    PATH_SUFFIXES "lib"
)

# Set LUI_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(lui
    FOUND_VAR LUI_FOUND
    REQUIRED_VARS LUI_LIBRARY LUI_INCLUDE_DIR
)

if(LUI_FOUND)
    message(STATUS "Found the Panda3D LUI")

    add_library(lui UNKNOWN IMPORTED)

    if(EXISTS "${LUI_LIBRARY}")
        set_target_properties(lui PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LUI_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${LUI_LIBRARY}"
        )
    endif()

    # Make variables changeable to the advanced user
    mark_as_advanced(
        LUI_INCLUDE_DIR
        LUI_LIBRARY
    )
endif()
