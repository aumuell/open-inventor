IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoUpgraderInit.c++ \
	SoUpgrader.c++ \
	SoV1CustomNode.c++ \
	SoV1DrawStyle.c++ \
	SoV1Environment.c++ \
	SoV1IndexedTriangleMesh.c++ \
	SoV1LayerGroup.c++ \
	SoV1LightModel.c++ \
	SoV1Material.c++ \
	SoV1PackedColor.c++ \
	SoV1PickStyle.c++ \
	SoV1Separator.c++ \
	SoV1ShapeHints.c++ \
	SoV1Texture2.c++ \
	SoV1Text3.c++ \
	SoV1TextureCoordinateCube.c++ \
	SoV1TextureCoordinateCylinder.c++ \
	SoV1TextureCoordinateEnvironment.c++ \
	SoV1TextureCoordinatePlane.c++ \
	SoV1TextureCoordinateSphere.c++ \
	SoV2Text2.c++ \
	SoV2Text3.c++ \
	SoV2AsciiText.c++ \
	SoV2FontStyle.c++ \
	SoV2LOD.c++ \
	SoV2MaterialIndex.c++ \
	SoV2VertexProperty.c++ \
	SoV2WWWAnchor.c++ \
	SoV2WWWInline.c++

all:: $(OBJECTS)

include $(COMMONRULES)
