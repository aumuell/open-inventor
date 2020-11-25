set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_demo(gview
	gview.c++ DisplayGraph.c++ Error.c++ FieldEditor.c++
	GraphIcon.c++ GraphViewer.c++ NodeCreator.c++
)

add_custom_target(gviewIcons.iv.h.target ALL
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/wrapIv ${CMAKE_CURRENT_SOURCE_DIR}/gviewIcons.iv > gviewIcons.iv.h
    DEPENDS gviewIcons.iv
    BYPRODUCTS gviewIcons.iv.h
)
#add_dependencies(gview gviewIcons.h.iv)

install(FILES windmill.iv gviewIcons.iv
    DESTINATION ${IVDEMODATADIR})
