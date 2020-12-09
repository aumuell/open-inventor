set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(ivview
    ivview.c++

    ivviewMenus.h
)

target_link_libraries(ivview PRIVATE
    InventorWidget
    InventorXt
    Inventor
)

install(TARGETS ivview)
install(FILES ivview.about.pdf
    DESTINATION ${IVHELPDIR}/ivview)
