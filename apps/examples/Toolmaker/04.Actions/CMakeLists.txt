add_executable(printVolume
    printVolume.c++ GetVolumeAction.c++
    GetVolumeAction.h
)
target_link_libraries(printVolume Inventor) # no InventorXt required
