macro(ADD_EXAMPLE APP)
    add_executable(${APP} ${ARGN})
    target_link_libraries(${APP} InventorXt Inventor)
    #install(TARGETS ${APP} DESTINATION ${IVDEMOBINDIR})
endmacro()

add_subdirectory(Mentor)
add_subdirectory(Toolmaker)
