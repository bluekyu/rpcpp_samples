# Author: Younguk Kim (bluekyu)


# Configure dubugging information for the target.
#
# If you want to strip private debugging information (ex, sources), you can set 'postfix' string.
# Then, you will get stripped debugging information and a full debugging file.
#
# configure_debugging_information(TARGET <target> [POSTFIX <postfix>])
# @param    TARGET      target variable
# @param    POSTFIX     postfix of file of full (private) debugging information
function(configure_debugging_information)
    cmake_parse_arguments(ARG "" "TARGET;POSTFIX" "CONFIGURATIONS" ${ARGN})

    if(NOT ARG_CONFIGURATIONS)
        message(FATAL_ERROR "At least one configuration should be used in CONFIGURATIONS.")
    endif()

    if(ARG_POSTFIX)
        set(private_debinfo_postfix ${ARG_POSTFIX})
        message("Target '${ARG_TARGET}' is set to strip private debugging information.")
    endif()

    foreach(config ${ARG_CONFIGURATIONS})
        string(TOUPPER ${config} config_upper)

        if(MSVC)
            define_property(TARGET PROPERTY DEBINFO_STRIP_PATH_${config_upper}
                BRIEF_DOCS "The path of STRIPPED debugging information file."
                FULL_DOCS "The path of STRIPPED debugging information file."
            )

            # get PDB_OUTPUT_DIRECTORY
            get_target_property(PDB_OUTPUT_DIRECTORY ${ARG_TARGET} PDB_OUTPUT_DIRECTORY_${config_upper})
            if(NOT PDB_OUTPUT_DIRECTORY)
                get_target_property(PDB_OUTPUT_DIRECTORY ${ARG_TARGET} PDB_OUTPUT_DIRECTORY)
                if(NOT PDB_OUTPUT_DIRECTORY)
                    get_target_property(PDB_OUTPUT_DIRECTORY ${ARG_TARGET} LIBRARY_OUTPUT_DIRECTORY_${config_upper})
                    if(NOT PDB_OUTPUT_DIRECTORY)
                        get_target_property(PDB_OUTPUT_DIRECTORY ${ARG_TARGET} LIBRARY_OUTPUT_DIRECTORY)
                        if(NOT PDB_OUTPUT_DIRECTORY)
                            set(PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
                            if(NOT CMAKE_BUILD_TYPE)
                                set(PDB_OUTPUT_DIRECTORY "${PDB_OUTPUT_DIRECTORY}/${config}")
                            endif()
                        endif()
                    endif()
                endif()
            endif()

            # get PDB_NAME
            get_target_property(PDB_NAME ${ARG_TARGET} PDB_NAME_${config_upper})
            if(NOT PDB_NAME)
                get_target_property(PDB_NAME ${ARG_TARGET} PDB_NAME)
                if(NOT PDB_NAME)
                    get_target_property(PREFIX ${ARG_TARGET} PREFIX)
                    if(NOT PREFIX)
                        set(PREFIX "")
                    endif()

                    get_target_property(_OUTPUT_NAME ${ARG_TARGET} OUTPUT_NAME_${config_upper})
                    if(NOT _OUTPUT_NAME)
                        get_target_property(_OUTPUT_NAME ${ARG_TARGET} OUTPUT_NAME)
                        if(NOT _OUTPUT_NAME)
                            get_target_property(_OUTPUT_NAME ${ARG_TARGET} NAME)
                        endif()
                    endif()

                    get_target_property(POSTFIX ${ARG_TARGET} ${config_upper}_POSTFIX)
                    if(NOT POSTFIX)
                        set(POSTFIX "")
                    endif()

                    set(PDB_NAME "${PREFIX}${_OUTPUT_NAME}${POSTFIX}")
                endif()
            endif()

            if(private_debinfo_postfix)
                set(DEBINFO_STRIP_PATH "${PDB_OUTPUT_DIRECTORY}/${PDB_NAME}.pdb")

                set_target_properties(${ARG_TARGET} PROPERTIES DEBINFO_STRIP_PATH_${config_upper} "${DEBINFO_STRIP_PATH}")
                set_target_properties(${ARG_TARGET} PROPERTIES PDB_NAME_${config_upper} "${PDB_NAME}${private_debinfo_postfix}")

                set_property(TARGET ${ARG_TARGET} APPEND_STRING PROPERTY LINK_FLAGS_${config_upper}
                    " /PDBSTRIPPED:${DEBINFO_STRIP_PATH} "
                )
            endif()
        else()
            if(strip_postfix)
                message(WARNING "Striping private debugging infomation is NOT working in this platform.")
            endif()
            return()
        endif()
    endforeach()
endfunction()


# Install debugging information files for the target configured by "configure_debugging_information"
#
# If a stripped debugging information exists, this installs only it.
# If "INSTALL_PRIVATE" is specified, this installs both the stripped debugging information
# and a full (private) debugging information.
#
# install_debugging_information(TARGET <target> DESTINATION <destination> [INSTALL_PRIVATE])
# @param    TARGET              Target variable
# @param    DESTINATION         Installed destination
# @param    INSTALL_PRIVATE     Install also full (private) debugging information
function(install_debugging_information)
    cmake_parse_arguments(ARG "INSTALL_PRIVATE" "TARGET;DESTINATION" "CONFIGURATIONS" ${ARGN})

    if(NOT ARG_CONFIGURATIONS)
        message(FATAL_ERROR "At least one configuration should be used in CONFIGURATIONS.")
    endif()

    if(ARG_INSTALL_PRIVATE)
        message("Target '${ARG_TARGET}' will install full (private) debugging information.")
    endif()

    foreach(config ${ARG_CONFIGURATIONS})
        if(MSVC)
            string(TOUPPER ${config} config_upper)

            get_target_property(DEBINFO_STRIP_PATH ${ARG_TARGET} DEBINFO_STRIP_PATH_${config_upper})

            if(DEBINFO_STRIP_PATH)
                install(FILES "${DEBINFO_STRIP_PATH}" DESTINATION "${ARG_DESTINATION}" CONFIGURATIONS ${config})
                if(ARG_INSTALL_PRIVATE)
                    install(FILES $<TARGET_PDB_FILE:${ARG_TARGET}> DESTINATION "${ARG_DESTINATION}" CONFIGURATIONS ${config})
                endif()
            else()
                install(FILES $<TARGET_PDB_FILE:${ARG_TARGET}> DESTINATION "${ARG_DESTINATION}" CONFIGURATIONS ${config})
            endif()
        endif()
    endforeach()
endfunction()
