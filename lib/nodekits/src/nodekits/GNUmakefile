IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/database/include \
	    -I../../include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/nodekits/include -DDEBUG

CXXFILES = SoAppearKit.c++ SoBaseKit.c++ \
	SoCameraKit.c++ SoLightKit.c++ \
	SoNkCatalog.c++ SoNkParts.c++ \
	SoNodeKit.c++ SoNodeKitListPart.c++ \
	SoSceneKit.c++ SoSeparatorKit.c++ \
	SoShapeKit.c++ SoWrapperKit.c++

all:: $(OBJECTS)

include $(COMMONRULES)
