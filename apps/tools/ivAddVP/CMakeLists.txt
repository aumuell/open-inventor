add_executable(ivAddVP
    ivAddVP.c++
)

target_link_libraries(ivAddVP PRIVATE Inventor)

install(TARGETS ivAddVP)
