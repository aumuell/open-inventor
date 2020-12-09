add_executable(ivnorm
	ivnorm.c++
        FindNormals.c++
	Faces.c++
        Edges.c++

        Edges.h
        Faces.h
        FindNormals.h
)

target_link_libraries(ivnorm PRIVATE Inventor)

install(TARGETS ivnorm)
