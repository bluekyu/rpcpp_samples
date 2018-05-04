# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)
project(rpcpp_snippets
    DESCRIPTION "Snippets of Render Pipeline C++"
    LANGUAGES NONE
)

# === configure ====================================================================================
# subdirectory
set(rpcpp_snippets_list
    "GUIs"
    "primitives"
    "texture_and_material"
)

foreach(snippet_id ${rpcpp_snippets_list})
    option(rpcpp_snippets_BUILD_${snippet_id} "Enable to build '${snippet_id}'" ON)
    if(rpcpp_snippets_BUILD_${snippet_id})
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${snippet_id}")
    endif()
endforeach()
# ==================================================================================================
