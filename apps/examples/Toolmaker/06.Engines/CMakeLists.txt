add_library(Engines OBJECT
    ConvertSFShortToSFFloat.c++
    FanIn.c++ MultFloatVec3f.c++
    SoComposeVec2f.c++ SoDecomposeVec2f.c++
    Ticker.c++

    ConvertSFShortToSFFloat.h
    FanIn.h MultFloatVec3f.h
    SoComposeVec2f.h SoDecomposeVec2f.h
    Ticker.h
)

target_link_libraries(Engines PRIVATE Inventor)
