add_executable(ivToIncludeFile
    ivToIncludeFile.c++
)
target_link_libraries(ivToIncludeFile PRIVATE Inventor)

add_sample(testResults
    testResults.c++
)
