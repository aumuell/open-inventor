set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

add_demo(SceneViewer
    SceneViewer.c++
    SoSceneViewer.c++
    SvManipList.c++

    SoSceneViewer.h
    SvManipList.h
    SoSceneMenu.h
)

target_link_libraries(SceneViewer PRIVATE InventorWidget)

install(FILES jackInTheBox.iv
    DESTINATION ${IVDEMODATADIR})

# install to bin in addition to demo directory
install(TARGETS SceneViewer)
