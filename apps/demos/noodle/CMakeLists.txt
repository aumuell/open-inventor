set(CYLINDER_DIR ../../nodes/GeneralizedCylinder)

add_demo(noodle
	NoodleSlider.c++
	Interface.c++ NoodleSurfaceGizmo.c++ WorldInfo.c++ LineManip.c++
	NoodleTextureGizmo.c++ noodle.c++ LineManipHilight.c++
	profile.c++
        ${CYLINDER_DIR}/NurbMaker.c++
	${CYLINDER_DIR}/Triangulator.c++
	${CYLINDER_DIR}/GeneralizedCylinder.c++
)

target_include_directories(noodle PRIVATE ${CYLINDER_DIR})

install(FILES defaultTexture.rgb
    DESTINATION ${IVDEMODATADIR})
