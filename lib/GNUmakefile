IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

LIBDSO = libInventor.so

LLDDSOOPTS += -L$(IVDEPTH)/libimage -L$(FLDIR)

LLDLIBS = \
	-ljpeg -limage $(FLLIB) \
        -lX11 -lm \
        -lGLU -lGL

OBJECTS = \
	./database/src/sb/projectors/SbProjectors.o \
	./database/src/sb/Sb.o \
	./database/src/so/actions/SoActions.o \
	./database/src/so/bundles/SoBundles.o \
	./database/src/so/caches/SoCaches.o \
	./database/src/so/details/SoDetails.o \
	./database/src/so/elements/SoElement.o \
	./database/src/so/elements/SoElements.o \
	./database/src/so/engines/SoEngines.o \
	./database/src/so/engines/SoConcatenate.o \
	./database/src/so/engines/SoEngineInit.o \
	./database/src/so/engines/SoFieldConverters.o \
	./database/src/so/engines/SoGate.o \
	./database/src/so/engines/SoSelectOne.o \
	./database/src/so/errors/SoErrors.o \
	./database/src/so/errors/SoErrorInit.o \
	./database/src/so/events/SoEventInit.o \
	./database/src/so/events/SoEvents.o \
	./database/src/so/fields/SoField.o \
	./database/src/so/fields/SoFieldContainer.o \
	./database/src/so/fields/SoFieldData.o \
	./database/src/so/fields/SoFieldInit.o \
	./database/src/so/fields/SoFields.o \
	./database/src/so/fields/SoGlobalField.o \
	./database/src/so/nodes/nurbs/clients/nurbsClient.o \
	./database/src/so/nodes/nurbs/libnurbs/libNurbs.o \
	./database/src/so/nodes/nurbs/SoNurbs.o \
	./database/src/so/nodes/SoNode.o \
	./database/src/so/nodes/SoNodeInit.o \
	./database/src/so/nodes/SoAnnotation.o \
	./database/src/so/nodes/SoArray.o \
	./database/src/so/nodes/SoAsciiText.o \
	./database/src/so/nodes/SoBaseColor.o \
	./database/src/so/nodes/SoBlinker.o \
	./database/src/so/nodes/SoCallback.o \
	./database/src/so/nodes/SoCamera.o \
	./database/src/so/nodes/SoClipPlane.o \
	./database/src/so/nodes/SoColorIndex.o \
	./database/src/so/nodes/SoComplexity.o \
	./database/src/so/nodes/SoCone.o \
	./database/src/so/nodes/SoCoordinate3.o \
	./database/src/so/nodes/SoCube.o \
	./database/src/so/nodes/SoCylinder.o \
	./database/src/so/nodes/SoCoordinate4.o \
	./database/src/so/nodes/SoDrawStyle.o \
	./database/src/so/nodes/SoDirectionalLight.o \
	./database/src/so/nodes/SoEnvironment.o \
	./database/src/so/nodes/SoFaceSet.o \
	./database/src/so/nodes/SoFile.o \
	./database/src/so/nodes/SoFont.o \
	./database/src/so/nodes/SoGroup.o \
	./database/src/so/nodes/SoEventCallback.o \
	./database/src/so/nodes/SoFontStyle.o \
	./database/src/so/nodes/SoIndexedFaceSet.o \
	./database/src/so/nodes/SoIndexedLineSet.o \
	./database/src/so/nodes/SoInfo.o \
	./database/src/so/nodes/SoIndexedNurbsCurve.o \
	./database/src/so/nodes/SoIndexedShape.o \
	./database/src/so/nodes/SoLOD.o \
	./database/src/so/nodes/SoLabel.o \
	./database/src/so/nodes/SoLight.o \
	./database/src/so/nodes/SoIndexedNurbsSurface.o \
	./database/src/so/nodes/SoLineSet.o \
	./database/src/so/nodes/SoLevelOfDetail.o \
	./database/src/so/nodes/SoMaterial.o \
	./database/src/so/nodes/SoLocateHighlight.o \
	./database/src/so/nodes/SoLightModel.o \
	./database/src/so/nodes/SoLinearProfile.o \
	./database/src/so/nodes/SoNormal.o \
	./database/src/so/nodes/SoMaterialBinding.o \
	./database/src/so/nodes/SoMultipleCopy.o \
	./database/src/so/nodes/SoNurbsCurve.o \
	./database/src/so/nodes/SoNurbsProfile.o \
	./database/src/so/nodes/SoMatrixTransform.o \
	./database/src/so/nodes/SoNurbsSurface.o \
	./database/src/so/nodes/SoPackedColor.o \
	./database/src/so/nodes/SoPendulum.o \
	./database/src/so/nodes/SoNonIndexedShape.o \
	./database/src/so/nodes/SoPathSwitch.o \
	./database/src/so/nodes/SoPointSet.o \
	./database/src/so/nodes/SoProfile.o \
	./database/src/so/nodes/SoNormalBinding.o \
	./database/src/so/nodes/SoIndexedTriangleStripSet.o \
	./database/src/so/nodes/SoQuadMesh.o \
	./database/src/so/nodes/SoRotation.o \
	./database/src/so/nodes/SoPickStyle.o \
	./database/src/so/nodes/SoRotor.o \
	./database/src/so/nodes/SoPointLight.o \
	./database/src/so/nodes/SoScale.o \
	./database/src/so/nodes/SoShape.o \
	./database/src/so/nodes/SoShuttle.o \
	./database/src/so/nodes/SoOrthographicCamera.o \
	./database/src/so/nodes/SoRotationXYZ.o \
	./database/src/so/nodes/SoSphere.o \
	./database/src/so/nodes/SoSwitch.o \
	./database/src/so/nodes/SoText2.o \
	./database/src/so/nodes/SoPerspectiveCamera.o \
	./database/src/so/nodes/SoProfileCoordinate2.o \
	./database/src/so/nodes/SoProfileCoordinate3.o \
	./database/src/so/nodes/SoResetTransform.o \
	./database/src/so/nodes/SoSeparator.o \
	./database/src/so/nodes/SoShapeHints.o \
	./database/src/so/nodes/SoText3.o \
	./database/src/so/nodes/SoSpotLight.o \
	./database/src/so/nodes/SoTexture2.o \
	./database/src/so/nodes/SoTextureCoordinate2.o \
	./database/src/so/nodes/SoTransform.o \
	./database/src/so/nodes/SoTextureCoordinateBinding.o \
	./database/src/so/nodes/SoUnits.o \
	./database/src/so/nodes/SoTransformation.o \
	./database/src/so/nodes/readGIF.o \
	./database/src/so/nodes/SoTranslation.o \
	./database/src/so/nodes/SoTriangleStripSet.o \
	./database/src/so/nodes/SoUnknownNode.o \
	./database/src/so/nodes/SoVertexShape.o \
	./database/src/so/nodes/SoWWWAnchor.o \
	./database/src/so/nodes/SoTextureCoordinateDefault.o \
	./database/src/so/nodes/SoVertexProperty.o \
	./database/src/so/nodes/SoTextureCoordinateFunction.o \
	./database/src/so/nodes/SoTextureCoordinateEnvironment.o \
	./database/src/so/nodes/SoTextureCoordinatePlane.o \
	./database/src/so/nodes/SoTexture2Transform.o \
	./database/src/so/nodes/SoTransformSeparator.o \
	./database/src/so/nodes/SoWWWInline.o \
	./database/src/so/sensors/SoSensors.o \
	./database/src/so/upgraders/SoUpgraderInit.o \
	./database/src/so/upgraders/SoUpgrader.o \
	./database/src/so/upgraders/SoUpgraders.o \
	./database/src/so/So.o \
	./database/src/so/SoState.o \
	./database/src/so/linux.o \
	./interaction/src/actions/SoBoxHL.o \
	./interaction/src/actions/SoLineHL.o \
	./interaction/src/draggers/SoDraggerInit.o \
	./interaction/src/draggers/SoCenterballDragger.o \
	./interaction/src/draggers/SoDirectionalLightDragger.o \
	./interaction/src/draggers/SoDragPointDragger.o \
	./interaction/src/draggers/SoDragger.o \
	./interaction/src/draggers/SoHandleBoxDragger.o \
	./interaction/src/draggers/SoJackDragger.o \
	./interaction/src/draggers/SoPointLightDragger.o \
	./interaction/src/draggers/SoRotateCylindricalDragger.o \
	./interaction/src/draggers/SoRotateDiscDragger.o \
	./interaction/src/draggers/SoRotateSphericalDragger.o \
	./interaction/src/draggers/SoScale1Dragger.o \
	./interaction/src/draggers/SoScale2Dragger.o \
	./interaction/src/draggers/SoScale2UniformDragger.o \
	./interaction/src/draggers/SoScaleUniformDragger.o \
	./interaction/src/draggers/SoSpotLightDragger.o \
	./interaction/src/draggers/SoTabBoxDragger.o \
	./interaction/src/draggers/SoTabPlaneDragger.o \
	./interaction/src/draggers/SoTrackballDragger.o \
	./interaction/src/draggers/SoTransformBoxDragger.o \
	./interaction/src/draggers/SoTransformerDragger.o \
	./interaction/src/draggers/SoTranslate1Dragger.o \
	./interaction/src/draggers/SoTranslate2Dragger.o \
	./interaction/src/manips/SoManips.o \
	./interaction/src/nodekits/SoInteractionKit.o \
	./interaction/src/nodes/SoAntiSquish.o \
	./interaction/src/nodes/SoSurroundScale.o \
	./interaction/src/SoInteraction.o \
	./interaction/src/SoSceneMgr.o \
	./interaction/src/SoSelection.o \
	./nodekits/src/details/SoNodeKitDetail.o \
	./nodekits/src/nodekits/SoNodeKits.o \
	./nodekits/src/upgraders/SoNodeKitUpgraders.o \
	./nodekits/src/SoNodeKitPath.o

LARCHIVE = libInventor.a

SUBDIRS = database interaction nodekits

all: all_ivbin

HEADERS_SUBDIRS = $(SUBDIRS)

install: install_ivlibdso
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
