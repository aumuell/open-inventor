add_executable(ppp ppp.c++)
target_include_directories(ppp PRIVATE ../../lib/database/include)
target_link_libraries(ppp ${STANDARD_MATH_LIBRARY})
