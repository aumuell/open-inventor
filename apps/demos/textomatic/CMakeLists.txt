add_demo(textomatic
    textomatic.c++ TextGraph.c++ TextWrapper.c++
)

target_link_libraries(textomatic PRIVATE common Iconv::Iconv)
