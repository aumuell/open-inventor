IVDEPTH=../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../../
LDIRT = *.d $(GEN_FILES)
LCXXFLAGS += -I$(DEPTH)/include \
	    -Inurbs/head  \
	    -DLIBRARYBUILD  \
	    -I../../..//../../libimage \
	    -I../../../../../libFL/src -DDEBUG

.SUFFIXES: .pp

%.c++: %.pp
	../../../../../tools/ppp/ppp < $< > $@


CXXFILES = \
	SoNode.c++ \
	SoNodeInit.c++ \
	SoAnnotation.c++ SoArray.c++ \
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
	SoShapeHints.c++ \
	SoShuttle.c++ SoSphere.c++ \
	SoSpotLight.c++ SoSwitch.c++ \
	SoText2.c++ \
	SoTextureCoordinate2.c++ SoTextureCoordinateBinding.c++ \
	SoTextureCoordinateFunction.c++ SoTextureCoordinateDefault.c++ \
	SoTextureCoordinateEnvironment.c++ \
	SoTextureCoordinatePlane.c++ \
	SoTexture2.c++ SoTexture2Transform.c++ \
	SoTransform.c++ SoTransformSeparator.c++ \
	SoTransformation.c++ SoTranslation.c++ \
	SoTriangleStripSet.c++ SoUnits.c++ \
	SoUnknownNode.c++ SoVertexProperty.c++ \
	SoAsciiText.c++ \
	SoShape.c++ \
	SoText3.c++ \
	SoVertexShape.c++ SoWWWAnchor.c++ SoWWWInline.c++ 

#
# we have to define one or two of these two
# things...LIBRARYBUILD  GLBUILD
#

P_FILES = \
	SoCone.pp \
	SoCube.pp \
	SoCylinder.pp \
	SoFaceSet.pp \
	SoIndexedFaceSet.pp \
	SoIndexedLineSet.pp \
	SoIndexedTriangleStripSet.pp \
	SoLineSet.pp \
	SoPointSet.pp \
	SoQuadMesh.pp \
	SoSphere.pp \
	SoTriangleStripSet.pp \
	$(NULL)

GEN_FILES = $(P_FILES:.pp=.c++)

CFILES = \
	jdmaster.c \
	jcomapi.c jdapi.c jdatasrc.c jdcoefct.c jdcolor.c \
	jddctmgr.c jdhuff.c jdmainct.c jdmarker.c jdmerge.c \
	jdpostct.c jdsample.c jerror.c jidctflt.c jidctfst.c \
	jidctint.c jidctred.c jmemmgr.c jmemnobs.c jquant1.c \
	jquant2.c jutils.c readGIF.c \
	$(NULL) 

SUBDIRS = nurbs text

all: $(GEN_FILES) $(OBJECTS)
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

install: all
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(COMMONRULES)

clean clobber:
	rm -f *.o *.d
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

