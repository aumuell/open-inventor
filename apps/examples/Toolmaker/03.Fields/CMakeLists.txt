add_library(Fields OBJECT
    MFDouble.c++ SFDouble.c++
    MFDouble.h SFDouble.h
)

target_link_libraries(Fields PRIVATE Inventor)
