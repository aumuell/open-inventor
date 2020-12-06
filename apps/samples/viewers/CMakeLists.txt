macro(TRANSFORM_SOURCE outfiles NAMEPREF)
    get_filename_component(NAMESCRIPT ./nameScript ABSOLUTE)
    foreach(it ${ARGN})
        get_filename_component(inname ${it} NAME)
        string(REPLACE ${NAMEPREF} My outname ${inname})
        
        get_filename_component(infile ${it} ABSOLUTE)
        set(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outname})

        add_custom_command(OUTPUT ${outfile}
            COMMAND ${SED_EXECUTABLE} -f ${NAMESCRIPT} ${infile} > ${outfile}
            MAIN_DEPENDENCY ${infile}
            VERBATIM
        )
    set(${outfiles} ${${outfiles}} ${outfile})
  endforeach ()
endmacro()


set(H_PREFIX ${CMAKE_SOURCE_DIR}/libSoXt/include/Inventor/Xt/viewers/SoXt)
transform_source(HEADERS SoXt
    ${H_PREFIX}ExaminerViewer.h
    ${H_PREFIX}FullViewer.h
    ${H_PREFIX}Viewer.h
)

set(SRC_PREFIX ${CMAKE_SOURCE_DIR}/libSoXt/src/viewers/SoXt)
transform_source(SOURCES SoXt
    ${SRC_PREFIX}ExamVwr.c++
    ${SRC_PREFIX}FullVwr.c++
    ${SRC_PREFIX}Viewer.c++
    ${SRC_PREFIX}BitmapButton.c++
    ${SRC_PREFIX}BitmapButton.h
)

add_sample(viewerTest
    viewerTest.c++
    ${SOURCES}
    ${HEADERS}
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
target_include_directories(viewerTest PRIVATE ${CMAKE_SOURCE_DIR}/libSoXt/src/viewers)
