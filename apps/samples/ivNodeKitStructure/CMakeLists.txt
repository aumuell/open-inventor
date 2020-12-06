add_sample(ivNodeKitStructure
    ivNodeKitStructure.c++
)

if (CMAKE_DL_LIBS)
    target_link_libraries(ivNodeKitStructure PRIVATE ${CMAKE_DL_LIBS})
endif()
