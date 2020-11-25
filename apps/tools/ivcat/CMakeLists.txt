add_executable(ivcat
    ivcat.c++
)

target_link_libraries(ivcat PRIVATE Inventor)

install(TARGETS ivcat)
