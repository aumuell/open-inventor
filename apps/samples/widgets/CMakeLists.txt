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


set(H_PREFIX ${CMAKE_SOURCE_DIR}/libSoXt/include/_SoXt)
transform_source(SOXT_HEADERS _SoXt
    ${H_PREFIX}UIRegion.h
    ${H_PREFIX}ColorEditor.h
    ${H_PREFIX}ColorPatch.h
    ${H_PREFIX}ColorSlider.h
    ${H_PREFIX}ColorWheel.h
    ${H_PREFIX}Slider.h 
)

set(SOXT_PREFIX ${CMAKE_SOURCE_DIR}/libSoXt/src/mixedMode/SoXt)
transform_source(SOXT_SOURCES SoXt
    ${SOXT_PREFIX}UIRegion.c++
    ${SOXT_PREFIX}ColEd.c++
    ${SOXT_PREFIX}ColPatch.c++
    ${SOXT_PREFIX}ColSlider.c++
    ${SOXT_PREFIX}ColWheel.c++
    ${SOXT_PREFIX}Slider.c++
)

set(SO_PREFIX ${CMAKE_SOURCE_DIR}/libSoXt/src/mixedMode/So)
transform_source(SO_SOURCES So
    ${SO_PREFIX}FloatCBList.c++
    ${SO_PREFIX}FloatCallbackList.h 
)

if(USE_INTERNAL_GLW)
    set(GLW_SOURCES
	GLwDrawA.c++
	GLwMDrawA.c++
    )
else()
    set(GLW_SOURCES)
endif()

add_library(InventorWidget STATIC
	MyUIRegion.c++
	MyColEd.c++
	MyColPatch.c++
	MyColSlider.c++
	MyColWheel.c++
	MyDropSite.c++
	MyFileRead.c++
	MySlider.c++
	MyThumbWhl.c++
	MyFloatCBList.c++
	MySimpleMtlEditor.c++
	MyMtlPal.c++
	MyTextureEd.c++

        ${SOXT_SOURCES}
        ${SO_SOURCES}
        ${SOXT_HEADERS}

        ${GLW_SOURCES}
)

if(USE_INTERNAL_GLW)
    target_compile_definitions(InventorWidget PRIVATE INTERNALGLW)
endif()
target_include_directories(InventorWidget PUBLIC
    ${MOTIF_INCLUDE_DIR}
    X11::Xt
    X11::X11
    ${CMAKE_SOURCE_DIR}/libSoXt/include
    ${CMAKE_SOURCE_DIR}/lib/database/include
    INTERFACE .
)
target_link_libraries(InventorWidget PRIVATE
    Inventor
    InventorXt
)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(componentTest
    componentTest.c++
)
target_link_libraries(componentTest PRIVATE
    InventorWidget
    Inventor
    InventorXt
)
