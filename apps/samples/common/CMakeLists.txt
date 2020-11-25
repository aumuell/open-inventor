add_library(common STATIC
    Useful.c++ LineManip.c++ LineManipHilight.c++
)

target_link_libraries(common PRIVATE
    Inventor
)

target_include_directories(common INTERFACE
    .
)
