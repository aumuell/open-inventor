set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

add_demo(SceneViewer
    SceneViewer.c++
    SoSceneViewer.c++
    SvManipList.c++
)

target_link_libraries(SceneViewer PRIVATE InventorWidget)

install(FILES jackInTheBox.iv
    DESTINATION ${IVDEMODATADIR})

install(TARGETS SceneViewer)
