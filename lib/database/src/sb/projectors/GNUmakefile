IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SbCylinderPlaneProjector.c++ \
	SbCylinderProjector.c++ \
	SbCylinderSectionProjector.c++ \
	SbCylinderSheetProjector.c++ \
	SbLineProjector.c++ \
	SbPlaneProjector.c++ \
	SbProjector.c++ \
	SbSpherePlaneProjector.c++ \
	SbSphereProjector.c++ \
	SbSphereSectionProjector.c++ \
	SbSphereSheetProjector.c++

all:: $(OBJECTS)

include $(COMMONRULES)
