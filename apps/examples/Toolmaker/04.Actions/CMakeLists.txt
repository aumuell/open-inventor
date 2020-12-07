add_executable(printVolume
    printVolume.c++ GetVolumeAction.c++
)
target_link_libraries(printVolume Inventor) # no InventorXt required
