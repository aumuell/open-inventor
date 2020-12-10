add_library(Elements OBJECT
    TemperatureElement.c++
    TemperatureElement.h
)

target_link_libraries(Elements PRIVATE Inventor)
