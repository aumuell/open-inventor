set(IVMANDIR ${CMAKE_SOURCE_DIR}/doc/man/ivman)
set(TREEFILE ${IVMANDIR}/manTree)
file(READ "${TREEFILE}" TREEFILE_CONTENT)
# Replace newlines with semicolon so that it is treated as a list by CMake
# Will need to escape existing semicolons beforehand if file contains any.
string(REPLACE "\n" ";" TREEFILE_LINES "${TREEFILE_CONTENT}")

macro(ADD_IVMAN)
    set(outfiles)
    foreach(it ${ARGN})
        get_filename_component(outbase ${it} NAME_WE)
        get_filename_component(infile ${it} ABSOLUTE)
        set(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outbase}.3iv)

        foreach(_line ${TREEFILE_LINES})
            string(REGEX MATCH "^${outbase} .*" BASECLASSES_LINE ${_line})
            if (NOT "${BASECLASSES_LINE}" STREQUAL "")
                string(REPLACE " " ";" BASECLASSES "${BASECLASSES_LINE}")
                break()
            endif()
        endforeach()

        set(inputfiles)
        foreach(_class ${BASECLASSES})
            file(GLOB inputfile "${CMAKE_SOURCE_DIR}/doc/man/man3/ivm/*/${_class}.ivm")
            if (inputfile)
                set(inputfiles ${inputfiles} ${inputfile})
            endif()
        endforeach()

        add_custom_command(OUTPUT ${outfile}
            COMMAND ivman ${inputfiles} > ${outfile}
            MAIN_DEPENDENCY ${infile}
            VERBATIM)
        set(outfiles ${outfiles} ${outfile})
        add_custom_target(${outbase}.3iv.target ALL DEPENDS ${outfile})
    endforeach()

    #add_custom_target(man3 ALL DEPENDS ${outfiles})

    install(FILES ${outfiles} DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)
endmacro()

add_subdirectory(actions)
add_subdirectory(basics)
add_subdirectory(components)
add_subdirectory(details)
add_subdirectory(devices)
add_subdirectory(draggers)
add_subdirectory(engines)
add_subdirectory(errors)
add_subdirectory(events)
add_subdirectory(fields)
add_subdirectory(general)
add_subdirectory(interaction)
add_subdirectory(manips)
add_subdirectory(nodekits)
add_subdirectory(nodes)
add_subdirectory(projectors)
add_subdirectory(sensors)
add_subdirectory(viewers)
