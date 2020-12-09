add_executable(ivman
    Main.c++
    ClassDef.c++
    Man.c++
    Reader.c++
    Writer.c++
    BuildIssues.c++

    Man.h
)

target_link_libraries(ivman PRIVATE
    Inventor
    image
)
