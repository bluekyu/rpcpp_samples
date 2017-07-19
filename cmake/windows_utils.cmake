# Author: Younguk Kim (bluekyu)
# Date  : 2017-07-19

cmake_minimum_required(VERSION 3.4)

# add_longpath_manifest function.
#
# This function genereates "longpath.manifest" file if it does not exist.
# And then, it adds the manifest file to given target as source.
# The manifest file has "longPathAware" tag for Win32 application.
# see https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
function(add_longpath_manifest target_)
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
