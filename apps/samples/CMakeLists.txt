macro(ADD_SAMPLE APP)
    add_executable(${APP} ${ARGN})
    target_link_libraries(${APP} PRIVATE InventorXt Inventor)
    #install(TARGETS ${APP} DESTINATION ${IVDEMOBINDIR})
endmacro()

add_subdirectory(ivToInclude)
add_subdirectory(widgets)

if (INVENTOR_DEMOS)
    add_subdirectory(common)
endif()

if (INVENTOR_SAMPLES)
    add_subdirectory(4view)
    add_subdirectory(clock)
    add_subdirectory(ivNodeKitStructure)
    add_subdirectory(linkages)
    add_subdirectory(lod)
    add_subdirectory(manip)
    add_subdirectory(motif)
    add_subdirectory(spaceball)
    add_subdirectory(twoDisplays)
    add_subdirectory(viewers)
    add_subdirectory(vortex)
endif()
