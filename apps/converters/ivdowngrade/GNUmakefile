IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = ivdowngrade

CXXFILES = \
	main.c++ \
	downgradeToV1.c++ \
	downgradeToV2.c++ \
	SoEnvironmentV1.c++ \
	SoPickStyleV1.c++ \
	SoShapeHintsV1.c++ \
	SoTextureCoordinatePlaneV1.c++ \
	SoIndexedTriangleMeshV1.c++ \
	SoWWWAnchorV2.c++ \
	SoWWWInlineV2.c++ \
	SoAsciiTextV2.c++ \
	SoFontStyleV2.c++ \
	SoPackedColorV2.c++ \
	SoVertexPropertyV2.c++ \
	SoLODV2.c++ \
	SoText2V2.c++ \
	SoText3V2.c++

LLDLIBS = -lInventor

all: all_ivbin

install: install_ivbin

include $(IVCOMMONRULES)
