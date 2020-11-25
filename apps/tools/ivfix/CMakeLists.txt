add_executable(ivfix
    ivfix.c++
    IfBuilder.c++
    IfCollector.c++
    IfCondenser.c++
    IfFixer.c++
    IfFlattener.c++
    IfHasher.c++
    IfHolder.c++
    IfMerger.c++
    IfReplacer.c++
    IfReporter.c++
    IfShape.c++
    IfShapeList.c++
    IfSorter.c++
    IfStripper.c++
    IfTypes.c++
    IfWeeder.c++
)

target_link_libraries(ivfix PRIVATE Inventor)

install(TARGETS ivfix)
