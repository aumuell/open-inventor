add_demo(textomatic
    textomatic.c++ TextGraph.c++ TextWrapper.c++

    labels.h TextGraph.h TextWrapper.h
)

target_link_libraries(textomatic PRIVATE common Iconv::Iconv)
