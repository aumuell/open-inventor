IVDEPTH = ../../../../..
include $(IVDEPTH)/make/ivcommondefs

LCXXINCS += -I$(IVDEPTH)/libFL/src -Inurbs/head
LCXXDEFS += -DLIBRARYBUILD

CFILES = readGIF.c 

CXXFILES = \
	SoNode.c++ \
	SoNodeInit.c++ \
	SoAnnotation.c++ SoArray.c++ SoAsciiText.c++ \
	SoBaseColor.c++ SoBlinker.c++ \
	SoCallback.c++ SoCamera.c++ \
	SoClipPlane.c++ SoColorIndex.c++ \
	SoComplexity.c++ SoCone.c++ \
	SoCoordinate3.c++ SoCoordinate4.c++ \
	SoCube.c++ SoCylinder.c++ \
	SoDirectionalLight.c++ SoDrawStyle.c++ \
	SoEnvironment.c++ SoEventCallback.c++ \
	SoFaceSet.c++ SoFile.c++ \
	SoFont.c++ SoFontStyle.c++ SoGroup.c++ \
	SoIndexedFaceSet.c++ SoIndexedLineSet.c++ \
	SoIndexedNurbsCurve.c++ SoIndexedNurbsSurface.c++ \
	SoIndexedTriangleStripSet.c++ SoIndexedShape.c++ \
	SoInfo.c++ SoLabel.c++ \
	SoLevelOfDetail.c++ SoLOD.c++ SoLocateHighlight.c++ SoLight.c++ \
	SoLightModel.c++ SoLineSet.c++ \
	SoLinearProfile.c++ SoMaterial.c++ \
	SoMaterialBinding.c++ \
	SoMatrixTransform.c++ SoMultipleCopy.c++ \
	SoNonIndexedShape.c++ SoNormal.c++ \
	SoNormalBinding.c++ SoNurbsCurve.c++ \
	SoNurbsProfile.c++ SoNurbsSurface.c++ \
	SoOrthographicCamera.c++ SoPackedColor.c++ \
	SoPathSwitch.c++ SoPendulum.c++ \
	SoPerspectiveCamera.c++ SoPickStyle.c++ \
	SoPointLight.c++ SoPointSet.c++ \
	SoProfile.c++ SoProfileCoordinate2.c++ \
	SoProfileCoordinate3.c++ SoQuadMesh.c++ \
	SoResetTransform.c++ SoRotation.c++ \
	SoRotationXYZ.c++ SoRotor.c++ \
	SoScale.c++ SoSeparator.c++ \
	SoShape.c++ SoShapeHints.c++ \
	SoShuttle.c++ SoSphere.c++ \
	SoSpotLight.c++ SoSwitch.c++ \
	SoText2.c++ SoText3.c++ \
	SoTextureCoordinate2.c++ SoTextureCoordinateBinding.c++ \
	SoTextureCoordinateFunction.c++ SoTextureCoordinateDefault.c++ \
	SoTextureCoordinateEnvironment.c++ \
	SoTextureCoordinatePlane.c++ \
	SoTexture2.c++ SoTexture2Transform.c++ \
	SoTransform.c++ SoTransformSeparator.c++ \
	SoTransformation.c++ SoTranslation.c++ \
	SoTriangleStripSet.c++ SoUnits.c++ \
	SoUnknownNode.c++ SoVertexProperty.c++ \
	SoVertexShape.c++ SoWWWAnchor.c++ SoWWWInline.c++ 

PPFILES = \
	SoCone.pp SoCube.pp SoCylinder.pp \
	SoFaceSet.pp SoIndexedFaceSet.pp \
	SoIndexedLineSet.pp SoIndexedTriangleStripSet.pp \
	SoLineSet.pp SoPointSet.pp SoQuadMesh.pp \
	SoSphere.pp SoTriangleStripSet.pp

SUBDIRS = nurbs

all install: $(SHAPEFILES) $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
