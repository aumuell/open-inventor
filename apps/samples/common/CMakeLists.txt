add_library(common STATIC
    Useful.c++ LineManip.c++ LineManipHilight.c++
    Useful.h LineManip.h InventorLogo.h
)

target_link_libraries(common PRIVATE
    Inventor
)

target_include_directories(common INTERFACE
    .
)
