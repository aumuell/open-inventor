set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_demo(gview
	gview.c++ DisplayGraph.c++ Error.c++ FieldEditor.c++
	GraphIcon.c++ GraphViewer.c++ NodeCreator.c++
        gviewIcons.iv.h
)

add_custom_command(OUTPUT gviewIcons.iv.h  
    COMMAND ivToIncludeFile gviewIcons < ${CMAKE_CURRENT_SOURCE_DIR}/gviewIcons.iv > gviewIcons.iv.h
    DEPENDS gviewIcons.iv
)

add_custom_target(gviewIcons.iv.h.target DEPENDS gviewIcons.iv.h)

add_dependencies(gview gviewIcons.iv.h.target)

install(FILES windmill.iv
    DESTINATION ${IVDEMODATADIR})
