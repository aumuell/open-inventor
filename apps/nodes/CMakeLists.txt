macro(ADD_PLUGIN PLUG)
    add_library(${PLUG} MODULE ${ARGN})
    target_link_libraries(${PLUG} PRIVATE InventorXt Inventor)
    # don't prepend lib to plugin file name
    set_target_properties(${PLUG} PROPERTIES PREFIX "")
    install(TARGETS ${PLUG} DESTINATION ${CMAKE_INSTALL_LIBDIR}/InventorDSO)
endmacro()

add_subdirectory(BarChart)
add_subdirectory(Decal)
add_subdirectory(GeneralizedCylinder)
