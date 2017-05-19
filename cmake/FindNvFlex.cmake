#.rst:
# FindNvFlex
# --------------
#
# FindNvFlex.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-19
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   NvFlex_FOUND            - True if NVIDIA Flex has been found and can be used
#
# and the following imported targets::
#
#   NvFlex::CUDA        - The NVIDIA Flex CUDA library
#   NvFlex::D3D         - The NVIDIA Flex D3D library

cmake_minimum_required(VERSION 3.6)

find_path(NvFlex_INCLUDE_DIR
    NAMES "NvFlex.h"
    HINTS "${NvFlex_ROOT}/include"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(NvFlex_arch_suffix "64")
else()
  set(NvFlex_arch_suffix "86")
endif()

set(NvFlex_COMPONENTS "NvFlexDevice")
set(NvFlex_GPU_COMPONENTS "NvFlex" "NvFlexExt")
set(NvFlex_CONFIGURATIONS "Release" "Debug")
set(NvFlex_GPU_TYPE "CUDA" "D3D")

foreach(component ${NvFlex_COMPONENTS})
    foreach(configuration ${NvFlex_CONFIGURATIONS})
        set(_lib_name ${component}${configuration})
        find_library(NvFlex_LIBRARY_${_lib_name}
            NAMES ${_lib_name}_x${NvFlex_arch_suffix}
            HINTS "${NvFlex_ROOT}/lib"
            PATH_SUFFIXES win${NvFlex_arch_suffix}
        )
        unset(_lib_name)
    endforeach()
endforeach()

foreach(component ${NvFlex_GPU_COMPONENTS})
    foreach(configuration ${NvFlex_CONFIGURATIONS})
        foreach(gpu_type ${NvFlex_GPU_TYPE})
            set(_lib_name ${component}${configuration}${gpu_type})
            find_library(NvFlex_LIBRARY_${_lib_name}
                NAMES ${_lib_name}_x${NvFlex_arch_suffix}
                HINTS "${NvFlex_ROOT}/lib"
                PATH_SUFFIXES win${NvFlex_arch_suffix}
            )
            unset(_lib_name)
        endforeach()
    endforeach()
endforeach()

# Set NvFlex_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NvFlex
    FOUND_VAR NvFlex_FOUND
    REQUIRED_VARS NvFlex_INCLUDE_DIR
)

if(NvFlex_FOUND)
    message(STATUS "Found the NVIDIA Flex")

    foreach(component ${NvFlex_COMPONENTS})
        if(NvFlex_LIBRARY_${component}Release)
            set(NvFlex_LIBRARY_${component} "${NvFlex_LIBRARY_${component}Release}")
        elseif()
            set(NvFlex_LIBRARY_${component} "${NvFlex_LIBRARY_${component}Debug}")
        endif()

        # Make variables changeable to the advanced user
        mark_as_advanced(
            NvFlex_LIBRARY_${component}Release
            NvFlex_LIBRARY_${component}Debug
        )
    endforeach()

    foreach(component ${NvFlex_GPU_COMPONENTS})
        foreach(gpu_type ${NvFlex_GPU_TYPE})
            if(NvFlex_LIBRARY_${component}Release${gpu_type})
                set(NvFlex_LIBRARY_${component}${gpu_type} "${NvFlex_LIBRARY_${component}Release${gpu_type}}")
            elseif()
                set(NvFlex_LIBRARY_${component}${gpu_type} "${NvFlex_LIBRARY_${component}Debug${gpu_type}}")
            endif()

            # Make variables changeable to the advanced user
            mark_as_advanced(
                NvFlex_LIBRARY_${component}Release${gpu_type}
                NvFlex_LIBRARY_${component}Debug${gpu_type}
            )
        endforeach()
    endforeach()

    # create targets of found components
    set(_NvFlex_INTERFACE_TARGET_DEPENDENCIES "")
    foreach(component ${NvFlex_COMPONENTS})
        add_library(NvFlex::${component} UNKNOWN IMPORTED)

        if(EXISTS "${NvFlex_LIBRARY_${component}}")
            set_target_properties(NvFlex::${component} PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                IMPORTED_LOCATION "${NvFlex_LIBRARY_${component}}"
            )
        endif()

        if(EXISTS "${NvFlex_LIBRARY_${component}Release}")
            set_property(TARGET NvFlex::${component} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(NvFlex::${component} PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
                IMPORTED_LOCATION_RELEASE "${NvFlex_LIBRARY_${component}Release}"
            )
        endif()

        if(EXISTS "${NvFlex_LIBRARY_${component}Debug}")
            set_property(TARGET NvFlex::${component} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(NvFlex::${component} PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
                IMPORTED_LOCATION_DEBUG "${NvFlex_LIBRARY_${component}Debug}"
            )
        endif()

        list(APPEND _NvFlex_INTERFACE_TARGET_DEPENDENCIES NvFlex::${component})
    endforeach()

    foreach(gpu_type ${NvFlex_GPU_TYPE})
        set(_NvFlex_INTERFACE_TARGET_DEPENDENCIES_${gpu_type} "")
    endforeach()
    foreach(component ${NvFlex_GPU_COMPONENTS})
        foreach(gpu_type ${NvFlex_GPU_TYPE})
            add_library(NvFlex::${gpu_type}::${component} UNKNOWN IMPORTED)

            if(EXISTS "${NvFlex_LIBRARY_${component}${gpu_type}}")
                set_target_properties(NvFlex::${gpu_type}::${component} PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                    IMPORTED_LOCATION "${NvFlex_LIBRARY_${component}${gpu_type}}"
                )
            endif()

            if(EXISTS "${NvFlex_LIBRARY_${component}Release${gpu_type}}")
                set_property(TARGET NvFlex::${gpu_type}::${component} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
                set_target_properties(NvFlex::${gpu_type}::${component} PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
                    IMPORTED_LOCATION_RELEASE "${NvFlex_LIBRARY_${component}Release${gpu_type}}"
                )
            endif()

            if(EXISTS "${NvFlex_LIBRARY_${component}Debug${gpu_type}}")
                set_property(TARGET NvFlex::${gpu_type}::${component} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
                set_target_properties(NvFlex::${gpu_type}::${component} PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
                    IMPORTED_LOCATION_DEBUG "${NvFlex_LIBRARY_${component}Debug${gpu_type}}"
                )
            endif()

            list(APPEND _NvFlex_INTERFACE_TARGET_DEPENDENCIES_${gpu_type} NvFlex::${gpu_type}::${component})
        endforeach()
    endforeach()

    # create interface target
    foreach(gpu_type ${NvFlex_GPU_TYPE})
        if(NOT TARGET NvFlex::${gpu_type})
            add_library(NvFlex::${gpu_type} INTERFACE IMPORTED)

            set_target_properties(NvFlex::${gpu_type} PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${NvFlex_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "${_NvFlex_INTERFACE_TARGET_DEPENDENCIES};${_NvFlex_INTERFACE_TARGET_DEPENDENCIES_${gpu_type}}"
            )
        endif()

        # Make variables changeable to the advanced user
        mark_as_advanced(
            NvFlex_LIBRARY_${gpu_type}
            NvFlex_LIBRARY_Release${gpu_type}
            NvFlex_LIBRARY_Debug${gpu_type}
        )
    endforeach()

    # Make variables changeable to the advanced user
    mark_as_advanced(NvFlex_INCLUDE_DIR)
endif()
