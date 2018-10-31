#.rst:
# FindOpenVR
# --------------
#
# FindOpenVR.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2018-05-11
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   OpenVR_FOUND            - True if OpenVR has been found and can be used
#
# and the following imported targets::
#
#   OpenVR::OpenVR          - The OpenVR library

cmake_minimum_required(VERSION 3.11.4)

if(NOT OpenVR_ROOT)
    # support vcpkg
    if(EXISTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
        set(OpenVR_ROOT "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    else()
        set(OpenVR_ROOT "")
    endif()
endif()
set(OpenVR_ROOT "${OpenVR_ROOT}" CACHE PATH "Hint for finding OpenVR root directory")

set(OpenVR_PLATFORM_PREFIX "")
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenVR_PLATFORM_PREFIX "win64")
    else()
        set(OpenVR_PLATFORM_PREFIX "win32")
    endif()
elseif(UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenVR_PLATFORM_PREFIX "linux64")
    else()
        set(OpenVR_PLATFORM_PREFIX "linux32")
    endif()
elseif(APPLE)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenVR_PLATFORM_PREFIX "osx64")
    else()
        set(OpenVR_PLATFORM_PREFIX "osx32")
    endif()
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}")
    return()
endif()

find_path(OpenVR_INCLUDE_DIR
    NAMES "openvr.h"
    HINTS "${OpenVR_ROOT}"
    PATH_SUFFIXES include headers
    NO_DEFAULT_PATH
)

find_library(OpenVR_LIBRARY
    NAMES openvr_api
    HINTS "${OpenVR_ROOT}"
    PATH_SUFFIXES lib "lib/${OpenVR_PLATFORM_PREFIX}"
    NO_DEFAULT_PATH
)

# Set OpenVR_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVR
    FOUND_VAR OpenVR_FOUND
    REQUIRED_VARS OpenVR_LIBRARY OpenVR_INCLUDE_DIR
)

if(OpenVR_FOUND)
    message(STATUS "Found the OpenVR")

    add_library(OpenVR::OpenVR UNKNOWN IMPORTED)

    if(EXISTS "${OpenVR_LIBRARY}")
        set_target_properties(OpenVR::OpenVR PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${OpenVR_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${OpenVR_LIBRARY}"
        )
    endif()

    # Make variables changeable to the advanced user
    mark_as_advanced(
        OpenVR_INCLUDE_DIR
        OpenVR_LIBRARY
        OpenVR_ROOT
    )
endif()
