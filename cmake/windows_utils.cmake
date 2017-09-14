# Author: Younguk Kim (bluekyu)
# Date  : 2017-07-19

cmake_minimum_required(VERSION 3.4)

# windows_add_longpath_manifest function.
#
# This function genereates "longpath.manifest" file if it does not exist.
# And then, it adds the manifest file to given target as source.
# The manifest file has "longPathAware" tag for Win32 application.
# see https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
#
# windows_add_longpath_manifest(<target>)
# @param    target      Target variable
function(windows_add_longpath_manifest target_)
    set(manifest_file_path "${CMAKE_BINARY_DIR}/src/windows/longpath.manifest")

    if(NOT EXISTS ${manifest_file_path})
        file(WRITE ${manifest_file_path}
            "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\n"
            "  <application xmlns=\"urn:schemas-microsoft-com:asm.v3\">\n"
            "    <windowsSettings>\n"
            "      <longPathAware xmlns=\"http://schemas.microsoft.com/SMI/2016/WindowsSettings\">true</longPathAware>\n"
            "    </windowsSettings>\n"
            "  </application>\n"
            "</assembly>\n"
        )
        message(STATUS "\"${manifest_file_path}\" file was created.")
    endif()

    target_sources(${target_} PRIVATE ${manifest_file_path})
endfunction()

# windows_add_delay_load function.
#
# This function add "/DELAYLOAD" option to link flags.
#
# windows_add_delay_load(<target> <dll_names> [<config>])
# @param    target      Target variable
# @param    dll_names   The list of name of dlls.
# @param    config      Configuration. ex) Release, Debug
function(windows_add_delay_load target_ dll_names_)
    if(ARGV2)
        string(TOUPPER ${ARGV2} configuration)
        set(configuration "_${configuration}")
    endif()

    set(flag "")
    foreach(dll_name ${dll_names_})
        set(flag "${flag} /DELAYLOAD:${dll_name}")
    endforeach()

    set_property(TARGET ${target_} APPEND_STRING PROPERTY LINK_FLAGS${configuration} "${flag} ")
endfunction()
