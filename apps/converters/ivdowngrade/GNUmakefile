IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -I/tmp/include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I../../../lib/database/include/ \
	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include \
	    -D__NO_MATH_INLINES

CXXFILES =  \
	SoAsciiTextV2.c++ \
	SoEnvironmentV1.c++ \
	SoFontStyleV2.c++ \
	SoIndexedTriangleMeshV1.c++ \
	SoLODV2.c++ \
	SoPackedColorV2.c++ \
	SoPickStyleV1.c++ \
	SoShapeHintsV1.c++ \
	SoText2V2.c++ \
	SoText3V2.c++ \
	SoTextureCoordinatePlaneV1.c++ \
	SoVertexPropertyV2.c++ \
	SoWWWAnchorV2.c++ \
	SoWWWInlineV2.c++ \
	downgradeToV1.c++ \
	downgradeToV2.c++ \
	main.c++


TARGETS = ivdowngrade

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib/ \
	-L../../samples/widgets \
	-L../../../libSoXt/ \
	-L/usr/X11R6/lib \
	-L/usr/X11R6/lib \
	-Wl,-rpath,../../libimage \
	-Wl,-rpath,../../../lib \
	-Wl,-rpath,../../../libSoXt

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src \
	-Wl,-rpath,../../../libFL/src
endif

LLDLIBS = \
	-lInventorXt -lInventor \
	-limage \
	-lFL -lGLU -lGL \
	-lInventorWidget \
	-lXm -lXt -lXext -lXmu -lXi -lX11 -ldl -lm

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/usr/bin

include $(COMMONRULES)

