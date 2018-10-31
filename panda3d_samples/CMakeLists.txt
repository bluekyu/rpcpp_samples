# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)
project(panda3d_samples
    LANGUAGES NONE
)

# === configure ====================================================================================
# warning about fixed function
if(EXISTS ${panda3d_ROOT})
    set(_panda3d_root "${panda3d_ROOT}")
elseif(EXISTS ${panda3d_ROOT_RELEASE})
    set(_panda3d_root "${panda3d_ROOT_RELEASE}")
else()
    set(_panda3d_root "${panda3d_ROOT_DEBUG}")
endif()
file(STRINGS "${_panda3d_root}/include/dtool_config.h"
    panda3d_dtool_SUPPORT_FIXED_FUNCTION
    REGEX "#define SUPPORT_FIXED_FUNCTION 1"
)
if(NOT panda3d_dtool_SUPPORT_FIXED_FUNCTION)
    message(AUTHOR_WARNING
        "  \n"
        "  Current Panda3D SDK does not support fixed function.\n"
        "  (SUPPORT_FIXED_FUNCTION is undefined)\n"
        "  Basic Panda3D samples may be rendered incorrectly.\n")
endif()

add_subdirectory("chorusline")
add_subdirectory("empty-wo-rpcpp")
add_subdirectory("looking-and-gripping")
add_subdirectory("music-box")
# ==================================================================================================

# === install ======================================================================================
# ==================================================================================================
