add_demo(qmorf
    qmorf.c++ QuadThing.c++ Background.c++ FlashBackground.c++

    QuadThing.h Background.h
)

foreach(APP quadSphere quadCube quadSheet)
    add_executable(${APP} ${APP}.c++ QuadThing.c++ QuadThing.h)
    target_link_libraries(${APP} PRIVATE Inventor)
    #install(TARGETS ${APP} DESTINATION ${IVDEMOBINDIR})
endforeach()
