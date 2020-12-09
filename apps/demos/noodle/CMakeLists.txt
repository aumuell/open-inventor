set(CYLINDER_DIR ../../nodes/GeneralizedCylinder)

add_demo(noodle
        noodle.c++
	profile.c++
	NoodleSlider.c++
	Interface.c++ NoodleSurfaceGizmo.c++ WorldInfo.c++ LineManip.c++
	NoodleTextureGizmo.c++ LineManipHilight.c++
        ${CYLINDER_DIR}/NurbMaker.c++
	${CYLINDER_DIR}/Triangulator.c++
	${CYLINDER_DIR}/GeneralizedCylinder.c++

        NoodleSlider.h
        Interface.h NoodleSurfaceGizmo.h WorldInfo.h LineManip.h
        NoodleTextureGizmo.h
        MyGrids.h PullDowns.h
)

target_include_directories(noodle PRIVATE ${CYLINDER_DIR})

install(FILES defaultTexture.rgb
    DESTINATION ${IVDEMODATADIR})
