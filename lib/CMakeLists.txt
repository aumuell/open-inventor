macro(ADD_PP_FILES outfiles)
    foreach(it ${ARGN})
        get_filename_component(outbase ${it} NAME_WE)
        get_filename_component(infile ${it} ABSOLUTE)
        set(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outbase}.c++)

        add_custom_command(OUTPUT ${outfile}
            COMMAND ppp < ${infile} > ${outfile}
            MAIN_DEPENDENCY ${infile}
            VERBATIM)
        set(${outfiles} ${${outfiles}} ${outfile})
    endforeach ()
endmacro()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(database/src/so/engines)
bison_target(CALCPARSE
    database/src/so/engines/SoCalcParse.y
    ${CMAKE_CURRENT_BINARY_DIR}/SoCalcParse.c++
)

add_pp_files(PP_SOURCES
    database/src/so/nodes/SoCone.pp
    database/src/so/nodes/SoCube.pp
    database/src/so/nodes/SoCylinder.pp
    database/src/so/nodes/SoFaceSet.pp
    database/src/so/nodes/SoIndexedFaceSet.pp
    database/src/so/nodes/SoIndexedLineSet.pp
    database/src/so/nodes/SoLineSet.pp
    database/src/so/nodes/SoPointSet.pp
    database/src/so/nodes/SoIndexedTriangleStripSet.pp
    database/src/so/nodes/SoQuadMesh.pp
    database/src/so/nodes/SoSphere.pp
    database/src/so/nodes/SoTriangleStripSet.pp
)

add_library(Inventor
    ${PP_SOURCES}
    database/src/sb/projectors/SbProjectors.c++
    database/src/sb/Sb.c++
    database/src/so/actions/SoActions.c++
    database/src/so/bundles/SoBundles.c++
    database/src/so/caches/SoCaches.c++
    database/src/so/details/SoDetails.c++
    database/src/so/elements/SoElement.c++
    database/src/so/elements/SoElements.c++
    database/src/so/engines/SoEngines.c++
    database/src/so/engines/SoConcatenate.c++
    database/src/so/engines/SoEngineInit.c++
    database/src/so/engines/SoFieldConverters.c++
    database/src/so/engines/SoGate.c++
    database/src/so/engines/SoSelectOne.c++
    database/src/so/errors/SoErrors.c++
    database/src/so/errors/SoErrorInit.c++
    database/src/so/events/SoEventInit.c++
    database/src/so/events/SoEvents.c++
    database/src/so/fields/SoField.c++
    database/src/so/fields/SoFieldContainer.c++
    database/src/so/fields/SoFieldData.c++
    database/src/so/fields/SoFieldInit.c++
    database/src/so/fields/SoFields.c++
    database/src/so/fields/SoGlobalField.c++
    database/src/so/nodes/nurbs/clients/nurbsClient.c++
    database/src/so/nodes/nurbs/libnurbs/libNurbs.c++
    database/src/so/nodes/nurbs/SoNurbs.c++
    database/src/so/nodes/SoNode.c++
    database/src/so/nodes/SoNodeInit.c++
    database/src/so/nodes/SoAnnotation.c++
    database/src/so/nodes/SoArray.c++
    database/src/so/nodes/SoAsciiText.c++
    database/src/so/nodes/SoBaseColor.c++
    database/src/so/nodes/SoBlinker.c++
    database/src/so/nodes/SoCallback.c++
    database/src/so/nodes/SoCamera.c++
    database/src/so/nodes/SoClipPlane.c++
    database/src/so/nodes/SoColorIndex.c++
    database/src/so/nodes/SoComplexity.c++
    database/src/so/nodes/SoCoordinate3.c++
    database/src/so/nodes/SoCoordinate4.c++
    database/src/so/nodes/SoDrawStyle.c++
    database/src/so/nodes/SoDirectionalLight.c++
    database/src/so/nodes/SoEnvironment.c++
    database/src/so/nodes/SoFile.c++
    database/src/so/nodes/SoFont.c++
    database/src/so/nodes/SoGroup.c++
    database/src/so/nodes/SoEventCallback.c++
    database/src/so/nodes/SoFontStyle.c++
    database/src/so/nodes/SoInfo.c++
    database/src/so/nodes/SoIndexedNurbsCurve.c++
    database/src/so/nodes/SoIndexedShape.c++
    database/src/so/nodes/SoLOD.c++
    database/src/so/nodes/SoLabel.c++
    database/src/so/nodes/SoLight.c++
    database/src/so/nodes/SoIndexedNurbsSurface.c++
    database/src/so/nodes/SoLevelOfDetail.c++
    database/src/so/nodes/SoMaterial.c++
    database/src/so/nodes/SoLocateHighlight.c++
    database/src/so/nodes/SoLightModel.c++
    database/src/so/nodes/SoLinearProfile.c++
    database/src/so/nodes/SoNormal.c++
    database/src/so/nodes/SoMaterialBinding.c++
    database/src/so/nodes/SoMultipleCopy.c++
    database/src/so/nodes/SoNurbsCurve.c++
    database/src/so/nodes/SoNurbsProfile.c++
    database/src/so/nodes/SoMatrixTransform.c++
    database/src/so/nodes/SoNurbsSurface.c++
    database/src/so/nodes/SoPackedColor.c++
    database/src/so/nodes/SoPendulum.c++
    database/src/so/nodes/SoNonIndexedShape.c++
    database/src/so/nodes/SoPathSwitch.c++
    database/src/so/nodes/SoProfile.c++
    database/src/so/nodes/SoNormalBinding.c++
    database/src/so/nodes/SoRotation.c++
    database/src/so/nodes/SoPickStyle.c++
    database/src/so/nodes/SoRotor.c++
    database/src/so/nodes/SoPointLight.c++
    database/src/so/nodes/SoScale.c++
    database/src/so/nodes/SoShape.c++
    database/src/so/nodes/SoShuttle.c++
    database/src/so/nodes/SoOrthographicCamera.c++
    database/src/so/nodes/SoRotationXYZ.c++
    database/src/so/nodes/SoSwitch.c++
    database/src/so/nodes/SoText2.c++
    database/src/so/nodes/SoPerspectiveCamera.c++
    database/src/so/nodes/SoProfileCoordinate2.c++
    database/src/so/nodes/SoProfileCoordinate3.c++
    database/src/so/nodes/SoResetTransform.c++
    database/src/so/nodes/SoSeparator.c++
    database/src/so/nodes/SoShapeHints.c++
    database/src/so/nodes/SoText3.c++
    database/src/so/nodes/SoSpotLight.c++
    database/src/so/nodes/SoTexture2.c++
    database/src/so/nodes/SoTextureCoordinate2.c++
    database/src/so/nodes/SoTransform.c++
    database/src/so/nodes/SoTextureCoordinateBinding.c++
    database/src/so/nodes/SoUnits.c++
    database/src/so/nodes/SoTransformation.c++
    database/src/so/nodes/readGIF.c
    database/src/so/nodes/SoTranslation.c++
    database/src/so/nodes/SoUnknownNode.c++
    database/src/so/nodes/SoVertexShape.c++
    database/src/so/nodes/SoWWWAnchor.c++
    database/src/so/nodes/SoTextureCoordinateDefault.c++
    database/src/so/nodes/SoVertexProperty.c++
    database/src/so/nodes/SoTextureCoordinateFunction.c++
    database/src/so/nodes/SoTextureCoordinateEnvironment.c++
    database/src/so/nodes/SoTextureCoordinatePlane.c++
    database/src/so/nodes/SoTexture2Transform.c++
    database/src/so/nodes/SoTransformSeparator.c++
    database/src/so/nodes/SoWWWInline.c++
    database/src/so/sensors/SoSensors.c++
    database/src/so/upgraders/SoUpgraderInit.c++
    database/src/so/upgraders/SoUpgrader.c++
    database/src/so/upgraders/SoUpgraders.c++
    database/src/so/So.c++
    database/src/so/SoState.c++
    database/src/so/linux.c++
    interaction/src/actions/SoBoxHL.c++
    interaction/src/actions/SoLineHL.c++
    interaction/src/draggers/SoDraggerInit.c++
    interaction/src/draggers/SoCenterballDragger.c++
    interaction/src/draggers/SoDirectionalLightDragger.c++
    interaction/src/draggers/SoDragPointDragger.c++
    interaction/src/draggers/SoDragger.c++
    interaction/src/draggers/SoHandleBoxDragger.c++
    interaction/src/draggers/SoJackDragger.c++
    interaction/src/draggers/SoPointLightDragger.c++
    interaction/src/draggers/SoRotateCylindricalDragger.c++
    interaction/src/draggers/SoRotateDiscDragger.c++
    interaction/src/draggers/SoRotateSphericalDragger.c++
    interaction/src/draggers/SoScale1Dragger.c++
    interaction/src/draggers/SoScale2Dragger.c++
    interaction/src/draggers/SoScale2UniformDragger.c++
    interaction/src/draggers/SoScaleUniformDragger.c++
    interaction/src/draggers/SoSpotLightDragger.c++
    interaction/src/draggers/SoTabBoxDragger.c++
    interaction/src/draggers/SoTabPlaneDragger.c++
    interaction/src/draggers/SoTrackballDragger.c++
    interaction/src/draggers/SoTransformBoxDragger.c++
    interaction/src/draggers/SoTransformerDragger.c++
    interaction/src/draggers/SoTranslate1Dragger.c++
    interaction/src/draggers/SoTranslate2Dragger.c++
    interaction/src/manips/SoManips.c++
    interaction/src/nodekits/SoInteractionKit.c++
    interaction/src/nodes/SoAntiSquish.c++
    interaction/src/nodes/SoSurroundScale.c++
    interaction/src/SoInteraction.c++
    interaction/src/SoSceneMgr.c++
    interaction/src/SoSelection.c++
    nodekits/src/details/SoNodeKitDetail.c++
    nodekits/src/nodekits/SoNodeKits.c++
    nodekits/src/upgraders/SoNodeKitUpgraders.c++
    nodekits/src/SoNodeKitPath.c++
)

set_property(SOURCE
    database/src/so/engines/SoEngines.c++
    APPEND PROPERTY OBJECT_DEPENDS lib/SoCalcParse.c++
)

foreach(file 
    database/src/so/upgraders/SoV1Text2.c++
    database/src/so/upgraders/SoV2Text2.c++
    database/src/so/nodes/SoText2.c++
    database/src/so/nodes/SoText3.c++)
    set_property(SOURCE ${file}
         APPEND PROPERTY COMPILE_DEFINITIONS IRIX_6
    )
endforeach()

target_compile_definitions(Inventor PRIVATE LIBRARYBUILD)

target_include_directories(Inventor PUBLIC
    database/include
    interaction/include
    nodekits/include
)

target_link_libraries(Inventor PUBLIC OpenGL::GL)
target_link_libraries(Inventor PUBLIC OpenGL::GLU)

target_include_directories(Inventor PRIVATE ../libFL/ang)
target_link_libraries(Inventor PRIVATE FL)

target_link_libraries(Inventor PRIVATE image)
target_include_directories(Inventor PRIVATE database/src/so/nodes/nurbs/head)

target_link_libraries(Inventor PRIVATE JPEG::JPEG)

target_link_libraries(Inventor PRIVATE X11::X11)

target_link_libraries(Inventor PRIVATE Iconv::Iconv)

if (CMAKE_DL_LIBS)
    target_link_libraries(Inventor PRIVATE ${CMAKE_DL_LIBS})
endif()

set_target_properties(Inventor PROPERTIES
    #VERSION ${CMAKE_PROJECT_VERSION}
    VERSION 0.0.0
    SOVERSION 0
)

install(TARGETS Inventor)

install(DIRECTORY database/include/Inventor/ interaction/include/Inventor/ nodekits/include/Inventor/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${IVHDRTOP}
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "glVertArray.h" EXCLUDE
    PATTERN "SoGLMaterialIndexElement.h" EXCLUDE
    PATTERN "SoMaterialIndex.h" EXCLUDE
    PATTERN "SoVertexArray.h" EXCLUDE
)
