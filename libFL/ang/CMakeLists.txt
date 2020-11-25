find_package(Freetype REQUIRED)

add_library(FL STATIC
    fl.c flclient.c flfreetype.c
)

set_property(TARGET FL PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(FL PRIVATE Freetype::Freetype)
target_include_directories(FL PRIVATE OpenGL::GL)
target_link_libraries(FL PRIVATE OpenGL::GL)

target_include_directories(FL PUBLIC ../src)
