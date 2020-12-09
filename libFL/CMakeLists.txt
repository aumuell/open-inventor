find_package(Freetype REQUIRED)

add_library(FLiv STATIC
    fl.c flclient.c flfreetype.c
    fl.h flclient.h flfreetype.h
)

set_property(TARGET FLiv PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(FLiv PRIVATE Freetype::Freetype)
target_link_libraries(FLiv PRIVATE OpenGL::GL)

target_include_directories(FLiv PUBLIC .)
