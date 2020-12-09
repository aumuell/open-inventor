add_executable(ivdowngrade
    main.c++
    downgradeToV1.c++
    downgradeToV2.c++
    SoEnvironmentV1.c++
    SoPickStyleV1.c++
    SoShapeHintsV1.c++
    SoTextureCoordinatePlaneV1.c++
    SoIndexedTriangleMeshV1.c++
    SoWWWAnchorV2.c++
    SoWWWInlineV2.c++
    SoAsciiTextV2.c++
    SoFontStyleV2.c++
    SoPackedColorV2.c++
    SoVertexPropertyV2.c++
    SoLODV2.c++
    SoText2V2.c++
    SoText3V2.c++

    SoAsciiTextV2.h
    SoEnvironmentV1.h
    SoFontStyleV2.h
    SoIndexedTriangleMeshV1.h
    SoLODV2.h
    SoPackedColorV2.h
    SoPickStyleV1.h
    SoShapeHintsV1.h
    SoText2V2.h
    SoText3V2.h
    SoTextureCoordinatePlaneV1.h
    SoVertexPropertyV2.h
    SoWWWAnchorV2.h
    SoWWWInlineV2.h
    Util.h
)

target_link_libraries(ivdowngrade PRIVATE Inventor)

install(TARGETS ivdowngrade)
