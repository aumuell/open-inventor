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

    IfAssert.h
    IfBuilder.h
    IfCollector.h
    IfCondenser.h
    IfFixer.h
    IfFlattener.h
    IfHasher.h
    IfHolder.h
    IfMerger.h
    IfReplacer.h
    IfReporter.h
    IfShape.h
    IfShapeList.h
    IfSorter.h
    IfStripper.h
    IfTypes.h
    IfWeeder.h
)

target_link_libraries(ivfix PRIVATE Inventor)

install(TARGETS ivfix)
