add_executable(ivinfo
    ivinfo.c++
)

target_link_libraries(ivinfo PRIVATE Inventor)

install(TARGETS ivinfo)
